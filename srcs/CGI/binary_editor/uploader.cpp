#include <iostream>
#include <map>
#include <fstream>
//THIS IS NOT A WORKING PART OF PROJECT - JUST A CPP FOR CGI BINARY

std::map<std::string, std::string> build_envp(char **env) {
    int size;
    int i;
    int equality_index;
    std::map<std::string, std::string> result;
    std::string tmp;
    char *tmp_str;

    size = 0;
    while (env[size]) {
        tmp_str = env[size];
        i = 0;
        tmp.clear();
        while (tmp_str[i]){
            tmp.push_back(tmp_str[i]);
            i++;
        }
        equality_index = tmp.find("=");
        result.insert(std::make_pair(tmp.substr(0, equality_index),
                       tmp.substr(equality_index + 1, std::string::npos)));
        size++;
    }
    return (result);
}

//int uploader(char **envp)
int main(int argc, char **argv, char **envp)
	{
        std::map<std::string, std::string> env_parsed;

        //parse envp
    if (argc && argv)
        env_parsed = build_envp(envp);

//    for(std::map<std::string, std::string>::iterator it = env_parsed.begin(); //DEBUG
//    it != env_parsed.end(); it++)
//    {
//        std::cout << "header: " << it->first << " name: " << it->second << std::endl;
//    }
//        std::cout << "header: " << env_parsed["PATH"] << std::endl; //DEBUG

//    char buf[256]; //DEBUG
//    getcwd(buf, 256); //DEBUG

//try to create a file
    std::fstream create_file( env_parsed["PATH_INFO"], std::ios_base::trunc | std::ios_base::in
                                                           | std::ios_base::out);
    if (!create_file.is_open()){
        std::cout << "Content-Type: text/plain";
        std::cout << "\r\n\r\n" << std::endl;
//        std::cout << env_parsed["PATH_INFO"] << std::endl; //DEBUG
//        std::cout << buf << std::endl; //DEBUG
        std::cout << "<!DOCTYPE html>"                              << std::endl;
        std::cout << "<html>"                                       << std::endl;
        std::cout << "<title>" << "Upload error:" << "</title>"     << std::endl;
        std::cout << "<body>"                                       << std::endl;
        std::cout << "<div>"                                        << std::endl;
        std::cout << "<h1 align=\"center\">";
        std::cout << "Upload error:" << " "  << "Can't open destination directory";
        std::cout << "</h1>"                                        << std::endl;
        std::cout << "<hr>"                                         << std::endl;
        std::cout << "</div>"                                       << std::endl;
        std::cout << "</body>"                                      << std::endl;
        std::cout << "</html>"                                      << std::endl;
        return 1;
    }

    //write to file
    std::string line;
    while (std::getline(std::cin, line)){
        create_file << line;
        if (!std::cin.eof())
            create_file << '\n';
    }
    create_file.close();

    //response for success with no content length for test
    std::cout << "Content-Type: text/plain\r\n";
    std::cout << "Content-Length: 161\r\n";
    std::cout << "\r\n";
    std::cout << "<html>\r\n";
    std::cout << "<head>\r\n";
    std::cout << "<title>Body bgcolor Attribute example</title>\n";
    std::cout << "</head>\r\n";
    std::cout << "<body bgcolor=\"#afafaf\">\n";
    std::cout << "<h1 align=\"center\">CGI upload is successful</h1>\n";
    std::cout << "</body>\n";
    std::cout << "</html>" << std::endl;
		return 0;
	}