#ifndef __GLOBALCOMMANDS_H__
#define __GLOBALCOMMANDS_H__

string ProcessCommandString(string & str);

string RunCommand(deque<string> & args);
string AddCommand(deque<string> & args);
string DeleteCommand(deque<string> & args);
string ListCommand(deque<string> & args);
string GetCommand(deque<string> & args);
string SetCommand(deque<string> & args);
string QuitCommand(deque<string> & args);

#endif
