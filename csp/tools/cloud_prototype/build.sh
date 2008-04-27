clear
gcc Cloud.cpp CloudBox.cpp CloudMath.cpp CloudRegistry.cpp CloudTextureFactory.cpp visitors/RemoveRedundantCloudSpritesVisitor.cpp visitors/UpdateCloudModelVisitor.cpp Program.cpp \
-o prototype -Xlinker -losg -losgViewer
