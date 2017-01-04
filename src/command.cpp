#include "command.h"


using std::vector;
using std::string;

Command::Command(){
  this->time = 0;
  this->device = "";
  this->value = 0;
}

Command::Command(int gtime, string gdevice, float gvalue){
  this->time = gtime;
  this->device = gdevice;
  this->value = gvalue;
}

Command::Command(string command)
{
  vector<string> res;
  split(command, ' ', res);
  this->time = atoi(res[0].c_str());
  this->device = res[1];
  this->value = atof(res[2].c_str());
}

bool Command::operator<(Command const & b)
{
  return this->time < b.time;
}

void Command::split(const string& s, char c, vector<string>& v)
{
   string::size_type i = 0;
   string::size_type j = s.find(c);

   while (j != string::npos) {
      v.push_back(s.substr(i, j-i));
      i = ++j;
      j = s.find(c, j);

      if (j == string::npos)
         v.push_back(s.substr(i, s.length()));
   }
}
