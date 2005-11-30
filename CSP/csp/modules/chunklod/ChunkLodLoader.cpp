/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; indent-tabs-mode: nil; -*- */

/*
 * ChunkLodLoader.cpp
 *
 * Author(s): Vladimir Vukicevic <vladimir@pobox.com>
 * 
 * The code follows closely after Thatcher Ulrich's sample ChunkLOD implementation.
 * See http://tulrich.com/geekstuff/chunklod.html for more information.
 * Original algorithm by Thatcher Ulrich, released into the public domain.
 *
 * The osgChunkLod library is open source and may be redistributed and/or modified
 * under the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more details.
 * 
 * Modifications by Mark Rose <mkrose@users.sourceforge.net>, May 2003,
 * see include/osgChunkLod/ChunkLod for details.
 *
 */


#include <csp/modules/chunklod/ChunkLodLoader>
#include <csp/modules/chunklod/ChunkLod>


namespace osgChunkLod {

ChunkLodLoader::ChunkLodLoader (ChunkLodTree* tree,
                                MmapFile* chu_file,
                                const TextureQuadTree* tqt)
{
	_tree = tree;
	_chunkfile = chu_file;
	_tqt = tqt;
	_usingThread = false;

	for (int i = 0; i < REQUEST_BUFFER_SIZE; i++)
	{
		_requestBuffer[i] = NULL;
		_retireBuffer[i]._chunk = NULL;
		_requestTextureBuffer[i] = NULL;
		_retireTextureBuffer[i]._chunk = NULL;
	}

#ifdef USE_WIN32_THREADS
	InitializeCriticalSection (&_hCritSection);
#elif defined(USE_PTHREADS)
	pthread_mutex_init(&_mutex, NULL);
#endif
}

ChunkLodLoader::~ChunkLodLoader ()
{
	useThread (false);
#if defined(USE_WIN32_THREADS)
	DeleteCriticalSection (&_hCritSection);
#elif defined(USE_PTHREADS)
	pthread_mutex_destroy(&_mutex);
#endif
}

void
ChunkLodLoader::useThread (bool use_thread)
{
	if (_usingThread == use_thread) {
		// nothing to do, we're already doing what's asked
		return; 
	}

#if defined(USE_WIN32_THREADS)
	if (_usingThread) {
		// we're currently using a thread, and we've been asked to stop
		_stopThread = TRUE;
		WaitForSingleObject (_hThread, INFINITE);
		CloseHandle (_hThread);
		_usingThread = FALSE;
	} else {
		// we're not using a thread, and we've been asked to start using one
		_usingThread = TRUE;
		_stopThread = FALSE;
		_hThread = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE) _loaderThreadFunc, this, 0, NULL);
		if (_hThread == NULL) {
			// thread creation failed; we'll continue using no thread
			_usingThread = FALSE;
		}
	}
#elif defined(USE_PTHREADS)
	if (_usingThread) {
		// we're currently using a thread, and we've been asked to stop
		_stopThread = true;
		pthread_join(_thread, 0);
		_usingThread = false;
	} else {
		// we're not using a thread, and we've been asked to start using one
		int result;
		_usingThread = true;
		_stopThread = false;
		pthread_attr_t        pta;
		pthread_attr_t        *attributes = NULL;
		struct sched_param    param;
		result = pthread_attr_init(&pta);
		if (!result) {
			memset(&param, 0, sizeof(param));
			std::cout << "SCHED PRIORITY = " << param.sched_priority << "\n";
			param.sched_priority -= 20;
			pthread_attr_setschedparam(&pta, &param);
			attributes = &pta;
		}
		result = pthread_create(&_thread, attributes, _loaderThreadFunc, (void *) this);
		if (result) {
			// thread creation failed; we'll continue using no thread
			_usingThread = false;
		}
		if (attributes != NULL) {
			pthread_attr_destroy(attributes);
		}
	}
#endif
}

void
ChunkLodLoader::syncLoader ()
{
	if (_usingThread) {
#if defined(USE_WIN32_THREADS)
		EnterCriticalSection (&_hCritSection);
#elif defined(USE_PTHREADS)
		pthread_mutex_lock (&_mutex);
#endif
	}

	// first handle unloads
	for (unsigned int i = 0; i < _unloadQueue.size(); i++) {
		ChunkLod* c = _unloadQueue[i];
		if (c->parent != NULL && c->parent->split == false) {
			c->unloadData();
		}
	}
	_unloadQueue.resize(0);

	// then unload textures
	for (unsigned int i = 0; i < _unloadTextureQueue.size(); i++) {
		ChunkLod* c = _unloadTextureQueue[i];
		if (c->parent != NULL) {
			assert (c->parent->texture.valid());
			assert (c->hasChildren() == false
				|| (!c->children[0]->texture.valid() &&
				!c->children[1]->texture.valid() &&
				!c->children[2]->texture.valid() &&
				!c->children[3]->texture.valid()));
			c->releaseTexture();
		}
	}
	_unloadTextureQueue.resize(0);

	// retire any service requests
	for (int i = 0; i < REQUEST_BUFFER_SIZE; i++) {
		retire_info& r = const_cast<retire_info&>(_retireBuffer[i]);
		if (r._chunk) {
			if (r._chunk->parent != NULL && r._chunk->parent->data == NULL) {
				delete r._chunk_data;
			} else {
				// ATI
				if (_tree->useVertexProgram()) {
					r._chunk_data->vertexInfo.convertToObjectArray ();
				}
				r._chunk->data = r._chunk_data;
			}
		}
		r._chunk = NULL;
		r._chunk_data = NULL;
	}

	// do the same for texture requests
	for (int i = 0; i < REQUEST_BUFFER_SIZE; i++) {
		retire_texture_info& r = const_cast<retire_texture_info&>(_retireTextureBuffer[i]);
		if (r._chunk) {
			if (r._chunk->parent != NULL && !r._chunk->parent->texture.valid()) {
				r._texture_image = NULL;
			} else {
				// make a texture for this chunk
				osg::Texture2D *texture = new osg::Texture2D ();
				texture->setImage (r._texture_image.get());
				texture->setUseHardwareMipMapGeneration (true);
				r._chunk->texture = texture;
			}
		}
		r._chunk = NULL;
		r._texture_image = NULL;
	}

	// now send new data to the loader thread, in
	// order of priority
	for (int i = 0; i < REQUEST_BUFFER_SIZE; i++) {
		_requestBuffer[i] = NULL;
	}

	int qsize = _loadQueue.size();
	if (qsize > 0) {
		int req_count = 0;
		qsort (&_loadQueue[0], qsize, sizeof (_loadQueue[0]), pending_load_request::compare);
		for (int i = 0; i < qsize; i++) {
			ChunkLod* c = _loadQueue[qsize - 1 - i]._chunk;
			if (c->data == NULL && (c->parent == NULL || c->parent->data != NULL)) {
				_requestBuffer[req_count++] = c;
				if (req_count >= REQUEST_BUFFER_SIZE) {
					break;
				}
			}
		}
		_loadQueue.resize(0);
	}

	for (int i = 0; i < REQUEST_BUFFER_SIZE; i++) {
		_requestTextureBuffer[i] = NULL;
	}

	int tqsize = _loadTextureQueue.size();
	if (tqsize > 0) {
		int req_count = 0;
		for (int i = 0; i < tqsize; i++) {
			ChunkLod* c = _loadTextureQueue[i];
			if (!c->texture.valid() && (c->parent == NULL || c->parent->texture.valid())) {
				_requestTextureBuffer[req_count++] = c;
				if (req_count >= REQUEST_BUFFER_SIZE) {
					break;
				}
			}
		}
		_loadTextureQueue.resize (0);
	}

	if (_usingThread) {
#if defined(USE_WIN32_THREADS)
		LeaveCriticalSection (&_hCritSection);
#elif defined(USE_PTHREADS)
		pthread_mutex_unlock(&_mutex);
#endif
	}

	// if threads are not in use, service the requests ourself
	if (_usingThread == false) {
		for (int count = 0; count < REQUEST_BUFFER_SIZE; count++) {
			bool loaded = _serviceData();
			if (loaded == false) break;
		}

		for (int count = 0; count < REQUEST_BUFFER_SIZE; count++) {
			bool loaded = _serviceTexture();
			if (loaded == false) break;
		}
	}
}

void
ChunkLodLoader::requestLoad (ChunkLod* chunk, float urgency)
{
//	printf ("requestLoad: chunk: 0x%08x (level %d %d,%d) urgency: %f\n",
//		chunk, chunk->level, chunk->x, chunk->z, urgency);
	if (chunk->parent == NULL || chunk->parent->data != NULL) {
		_loadQueue.push_back (pending_load_request (chunk, urgency));
	}
}

void
ChunkLodLoader::requestUnload (ChunkLod* chunk)
{
//	printf ("requestUnload: chunk: 0x%08x (level %d %d,%d)\n",
//		chunk, chunk->level, chunk->x, chunk->z);
	_unloadQueue.push_back (chunk);
}

void
ChunkLodLoader::requestLoadTexture (ChunkLod* chunk)
{
	if (chunk->parent == NULL || chunk->parent->texture.valid()) {
		_loadTextureQueue.push_back (chunk);
	}
}


void
ChunkLodLoader::requestUnloadTexture (ChunkLod* chunk)
{
	_unloadTextureQueue.push_back (chunk); 
}


//
// service data request
bool
ChunkLodLoader::_serviceData ()
{
	volatile ChunkLod* chunk_to_load = NULL;

	if (_usingThread) {
#if defined(USE_WIN32_THREADS)
		EnterCriticalSection (&_hCritSection);
#elif defined(USE_PTHREADS)
		pthread_mutex_lock (&_mutex);
#endif
	}

	for (int req = 0; req < REQUEST_BUFFER_SIZE; req++) {
		// FIXME TODO -- use a STL queue for this...
		chunk_to_load = _requestBuffer[0];
		for (int i = 0; i < REQUEST_BUFFER_SIZE - 1; i++) {
			_requestBuffer[i] = _requestBuffer[i+1];
		}
		_requestBuffer[REQUEST_BUFFER_SIZE-1] = NULL;

		if (chunk_to_load == NULL) break;

		bool in_retire_buffer = false;
		for (int i = 0; i < REQUEST_BUFFER_SIZE; i++) {
			if (_retireBuffer[i]._chunk == chunk_to_load) {
				chunk_to_load = NULL;
				in_retire_buffer = true;
				break;
			}
		}

		if (in_retire_buffer == false) break;
	}

	if (_usingThread) {
#if defined(USE_WIN32_THREADS)
		LeaveCriticalSection (&_hCritSection);
#elif defined(USE_PTHREADS)
		pthread_mutex_unlock (&_mutex);
#endif
	}

	if (chunk_to_load == NULL) {
		// nothing to do
		return false;
	}

	ChunkLodData* loaded_data = NULL;
	_chunkfile->seek (chunk_to_load->dataFilePosition);
	loaded_data = new ChunkLodData (_chunkfile);

	if (_usingThread) {
#if defined(USE_WIN32_THREADS)
		EnterCriticalSection (&_hCritSection);
#elif defined(USE_PTHREADS)
		pthread_mutex_lock (&_mutex);
#endif
	}

	for (int i = 0; i < REQUEST_BUFFER_SIZE; i++) {
		if (_retireBuffer[i]._chunk == NULL) {
			_retireBuffer[i]._chunk = (ChunkLod *) chunk_to_load;
			_retireBuffer[i]._chunk_data = loaded_data;
			break;
		}
	}

	if (_usingThread) {
#if defined(USE_WIN32_THREADS)
		LeaveCriticalSection (&_hCritSection);
#elif defined(USE_PTHREADS)
		pthread_mutex_unlock (&_mutex);
#endif
	}

	return true;
}

bool
ChunkLodLoader::_serviceTexture ()
{
	if (!_tqt) {
		// no textures, nothing to do
		return false;
	}

	volatile ChunkLod* chunk_to_load = NULL;

	if (_usingThread) {
#if defined(USE_WIN32_THREADS)
		EnterCriticalSection (&_hCritSection);
#elif defined(USE_PTHREADS)
		pthread_mutex_lock (&_mutex);
#endif
	}

	for (int req = 0; req < REQUEST_BUFFER_SIZE; req++) {
		chunk_to_load = _requestTextureBuffer[0];
		for (int i = 0; i < REQUEST_BUFFER_SIZE - 1; i++) {
			_requestTextureBuffer[i] = _requestTextureBuffer[i+1];
		}
		_requestTextureBuffer[REQUEST_BUFFER_SIZE-1] = NULL;

		if (chunk_to_load == NULL) break;

		bool in_retire_buffer = false;
		for (int i = 0; i < REQUEST_BUFFER_SIZE; i++) {
			if (_retireTextureBuffer[i]._chunk == chunk_to_load) {
				// already been serviced, don't do it again
				chunk_to_load = NULL;
				in_retire_buffer = true;
				break;
			}
		}
		if (in_retire_buffer == false) break;
	}

	if (_usingThread) {
#if defined(USE_WIN32_THREADS)
		LeaveCriticalSection (&_hCritSection);
#elif defined(USE_PTHREADS)
		pthread_mutex_unlock (&_mutex);
#endif
	}


	if (chunk_to_load == NULL) {
		return false;
	}

	osg::Image* texImage;
	texImage = _tqt->loadImage (chunk_to_load->level, chunk_to_load->x, chunk_to_load->z);

	if (_usingThread) {
#if defined(USE_WIN32_THREADS)
		EnterCriticalSection (&_hCritSection);
#elif defined(USE_PTHREADS)
		pthread_mutex_lock (&_mutex);
#endif
	}

	for (int i = 0; i < REQUEST_BUFFER_SIZE; i++) {
		if (_retireTextureBuffer[i]._chunk == NULL) {
			_retireTextureBuffer[i]._chunk = (ChunkLod *) chunk_to_load;
			_retireTextureBuffer[i]._texture_image = texImage;
			break;
		}
	}

	if (_usingThread) {
#if defined(USE_WIN32_THREADS)
		LeaveCriticalSection (&_hCritSection);
#elif defined(USE_PTHREADS)
		pthread_mutex_unlock (&_mutex);
#endif
	}
    
	return true;
}

//
// thread function
//

ChunkLodLoader::ThreadFuncReturn
ChunkLodLoader::_loaderThreadFunc (void *param)
{
	ChunkLodLoader *loader = (ChunkLodLoader *) param;
	while (!loader->_stopThread) {
		//std::cout << "IN LOADER THREAD\n";
		bool loaded = false;
		loaded = loaded || loader->_serviceData ();
		loaded = loaded || loader->_serviceTexture ();

		if (!loaded) {
			// sleep for a bit; this coild be done using a semaphore when data arrives
#if defined(USE_WIN32_THREADS)
			Sleep (10);
#elif defined(USE_PTHREADS)
			struct timespec tspec;
			tspec.tv_sec = 0;
			tspec.tv_nsec = 10000000;
			nanosleep(&tspec, &tspec);
#endif
		}
	}
	return 0;
}

} // namespace osgChunkLod 

