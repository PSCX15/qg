#include <vector>
#include <string>
#include <cstdlib>

using std::vector;
using std::string;

class Command
{
  public:
  int time;
  string device;
  float value;

  private:
  void split(const string& s, char c, vector<string>& v);

  public:
  Command();
  Command(int gtime, string gdevice, float gvalue);
  Command(std::string command);
  
  bool operator<(Command const & b);
  
};
