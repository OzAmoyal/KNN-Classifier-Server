#include <iostream>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
using namespace std;

bool isValidIpAddress(char *ipAddress)
{ // use the inet_pton routine to check if the ip address is valid.
  struct sockaddr_in sa;
  // if the address is valid reslut will get 1 value.
  int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
  return (result == 1);
}

// if port is invalid returs -1,otherwise returns the port number as int
int getPort(string port)
{
  std::size_t found = port.find_first_not_of("0123456789");
  if (found != std::string::npos)
  {
    return -1;
  }
  //data return a pointer to the start of the array of char *
  int portNum = atoi(port.data());
  if (portNum < 1024 || portNum > 65535)
  {
    return -1;
  }
  return portNum;
}
bool isValidDistance(std::string distance)
{
  return distance == "MIN" || distance == "MAN" || distance == "CHB" || distance == "AUC" || distance == "CAN";
}
// this function check if the input from the user is valid.
bool isValidDouble(std::string s)
{
  if (s.length() == 0)
  {
    return false;
  }
  // we want to get from the user only this characters "0123456789.-" as they represent Double number.
  std::size_t found = s.find_first_not_of("0123456789.-Ee");
  if (found != std::string::npos)
  {
    return false;
  }
  return true;
}

std::vector<std::string> getUserInput(string input)
{
  if (input == "-1")
  {
    throw std::out_of_range("close the program");
  }
  std::vector<std::string> vec;
  // split the input into tokens every time in the line that "" is appering the token store the string before the "".
  std::string token;
  // read the data into the stringstream
  std::istringstream iss(input);
  // split by any '' (space key)
  while (std::getline(iss, token, ' '))
  {

    // insert the input into the vector.
    vec.push_back(token);
  }
  if (vec.size() < 3)
  {
    throw std::invalid_argument("Not Enough Arguments");
  }
  return vec;
}
std::string getUserDistance(vector<std::string> input)
{
  if (!isValidDistance(input[input.size() - 2]))
  {
    throw std::invalid_argument("Invalid Distance argument");
  }
  return input[input.size() - 2];
}
int getUserK(vector<std::string> input)
{
  std::size_t found = input[input.size() - 1].find_first_not_of("0123456789");
  if (found != std::string::npos)
  {
    throw std::invalid_argument("Invalid K argument");
  }
  int k = stoi(input[input.size() - 1]);
  if (k <= 0)
  {
    throw std::invalid_argument("Invalid K argument");
  }
  return k;
}
// this function get the input from the user and then insert the valid input into vector.
std::vector<double> getUserVector()
{
  std::string input;
  // getting all the line fromn the user.
  std::getline(std::cin, input);
  std::vector<double> vec;
  // split the input into tokens every time in the line that "" is appering the token store the string before the "".
  std::string token;
  // read the data into the stringstream
  std::istringstream iss(input);
  // split by any '' (space key)
  while (std::getline(iss, token, ' '))
  {
    // check if the input is valid
    if (!isValidDouble(token))
    {
      throw std::invalid_argument("Invalid arguments for the Vector");
    }
    // insert the input into the vector.
    vec.push_back(std::stod(token));
  }
  return vec;
}
bool isValidVector(std::vector<string> vec)
{
  for (int i = 0; i < vec.size() - 2; i++)
  {
    if (!isValidDouble(vec[i]))
    {
      return false;
    }
  }
  return true;
}

int main(int argc, char *argv[])
{
  // first is the name of the program second is the ip and third is the ip
  if (argc != 3)
  {
    cout << "invalid input" << endl;
    return -1;
  }
  const int port_no =getPort(string(argv[2]));
  if (port_no < 0)
  {
    cout << "invalid input" << endl;
    return -1;
  }
  if(!isValidIpAddress(argv[1])){
    cout << "invalid input" << endl;
    return -1;
  }

  const char *ip_address = argv[1];
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror("error creating socket");
  }
  struct sockaddr_in sin;
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = inet_addr(ip_address);
  sin.sin_port = htons(port_no);
  if (connect(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
  {
    perror("error connecting to server");
  }
  while (true)
  {
    std::string input;
    // getting all the line fromn the user.
    std::getline(std::cin, input);
    std::string distance;
    int k;
    vector<string> userInput;
    try
    {
      userInput = getUserInput(input);
    }
    catch (std::out_of_range e)
    {
      close(sock);
      break;
    }
    catch (exception e)
    {
      cout << "invalid input" << endl;
      continue;
    }
    if (!isValidVector(userInput))
    {
      cout << "invalid input"<<endl;
      continue;
    }

    try
    {
      k = getUserK(userInput);
    }
    catch (exception e)
    {
      cout << "invalid input" << std::endl;
      continue;
    }
    try
    {
      distance = getUserDistance(userInput);
    }
    catch (exception e)
    {
      cout << "invalid input" << std::endl;
      continue;
    }

    int data_len = input.size();
    int sent_bytes = send(sock, input.data(), data_len, 0);
    if (sent_bytes < 0)
    {
      perror("error sending to server..");

    }
    char buffer[4096]={0};
    int expected_data_len = sizeof(buffer);
    int read_bytes = recv(sock, buffer, expected_data_len, 0);
    if (read_bytes == 0)
    {
      close(sock);
      return 0;
    }
    else if (read_bytes < 0)
    {
      perror("error recieving from server...");

    }
    else
    {
      cout << buffer << endl;
//buffer={0};
    }
  }
  return 0;
}