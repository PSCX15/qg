#include <vector>
#include <string>
#include <cstdlib>


using std::vector;
using std::string;

class Command
{
  public:
  int time;
  ros::Time stamp;
  string device;
  float value;
  float angle;

  private:
  void split(const string& s, char c, vector<string>& v);

  public:
  Command();
  Command(int gtime, string gdevice, float gvalue);
  Command(ros::Time gstamp, float gvalue, float gangle);
  Command(std::string command);
  
  bool operator<(Command const & b);
  
};
