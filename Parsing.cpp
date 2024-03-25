#include "Parsing.hpp"

void initLocData(DataLoc &data)
{
    data.path.clear();
    data.get = false;
    data.post = false;
    data.del = false;
    data.cgi = false;
    data.autoindex = false;

    data.index.first = false;
    data.index.second.clear();

    data.dirpost.first = false;
    data.dirpost.second.clear();

    data.ret.first = false;
    data.ret.second.clear();

    data.alias.first = false;
    data.alias.second.clear();
}

Location parseLocation(std::string prevLoc)
{
    DataLoc loc;
    size_t  index = 0, indexEnd = 0, indexStart = 0;
    int i;
    const char *locData[] = {"allow_cgi", "allow_autoindex", "index","dir_post" ,"return", "alias", "method", NULL};

    initLocData(loc);
    //index = prevLoc.find_first_not_of(" \t", index);
    if (index > prevLoc.size())
        throw std::invalid_argument(prevLoc);
    indexEnd = prevLoc.find_first_of("{", index);
    if (indexEnd > prevLoc.size())
        throw std::invalid_argument(prevLoc);
    indexEnd = prevLoc.find_last_not_of(" \t", indexEnd);
    if (indexEnd > prevLoc.size())
        throw std::invalid_argument(prevLoc);
    loc.path = prevLoc.substr(index, indexEnd);
    //std::cout<<"index == "<<index<<"index end == "<<indexEnd<<"locPath = "<<loc.path<<std::endl;
    index = prevLoc.find_first_of("{", indexEnd);
    if (index > prevLoc.size())
        throw std::invalid_argument(prevLoc);
    index++;
    while (index < prevLoc.size())
    {
        index = prevLoc.find_first_not_of(" \t", index);
        indexEnd = prevLoc.find_first_of(" \t", index);
        //indexEnd = prevLoc.find_last_not_of(" \t", indexEnd);
       // std::cout<<"index == "<<index<<" indexEnd == "<<indexEnd<<" test = "<<prevLoc.substr(index, indexEnd - index)<<std::endl;
       // std::cout<<"index au debut == "<<index<<"str = "<<prevLoc.c_str() + index<<std::endl;
        for (i = 0; locData[i]; i++)
        {
            if (prevLoc.compare(index, strlen(locData[i]), locData[i]) == 0)
            {
                //std::cout<<"i == "<<i<<"indexEnd = "<<indexEnd<<"content : "<<content.c_str() + indexEnd<<std::endl;
                index += strlen(locData[i]);
                indexStart = prevLoc.find_first_not_of(" \t", index);
         //       std::cout<<"i == "<<i<<"indexEnd = "<<indexEnd<<"content : "<<prevLoc.c_str() + indexEnd<<std::endl;
                break ;
            }
        }
        // if(i != method)
        // {
        //     indexEnd = prevLoc.find_first_of(";", index);
        //     if (indexEnd > prevLoc.size() || indexStart > prevLoc.size())
        //         throw std::invalid_argument("invalid file");
        //     indexEnd = prevLoc.find_last_of(" \t", index);
        //     std::cout<<"index == "<<index<<" indexEnd == "<<indexEnd<<" test = "<<prevLoc.substr(index, indexEnd - index)<<std::endl;
        // }
        // else
        // {
        // //    indexEnd = prevLoc.find_first_of("}", index);
        // //    std::cout<<"index == "<<index<<" indexEnd == "<<indexEnd<<" test = "<<prevLoc.substr(index, indexEnd - index)<<std::endl;
            
        // }
        //std::cout<<"avant index = "<<index<<std::endl;
        //std::cout<<"avant index End = "<<indexEnd<<std::endl;
        if (index >= prevLoc.size())
            throw std::invalid_argument("invalid file");
        if(i != method)
        {
            //std::cout<<"i = "<<i<<std::endl;
            indexEnd = prevLoc.find_first_of(";", index);
            if (indexEnd > prevLoc.size() || indexStart > prevLoc.size())
                break ;
            //indexEnd = prevLoc.find_last_of(" \t", index);
        }
        else
        {
          //  std::cout<<"i = "<<i<<std::endl;
            indexEnd = prevLoc.find_first_of("}", index);
            if (indexEnd > prevLoc.size() || indexStart > prevLoc.size())
                break ;
            //indexEnd = prevLoc.find_last_of(" \t", index);
        }
        //std::cout<<"index = "<<index<<std::endl;
        //std::cout<<"index End = "<<indexEnd<<std::endl;
        switch(i)
        {
            case cgi:
            {
                loc.cgi = true;
                break;
            }
            case autoindex:
            {
                loc.autoindex = true;
                break;
            }
            case _index:
            {
                std::cout<<"index == "<<prevLoc.substr(indexStart, indexEnd - indexStart)<<std::endl;
                loc.index.second = prevLoc.substr(indexStart, indexEnd - indexStart);
                std::cout<<"index second == "<<loc.index.second<<std::endl;
                if (!loc.index.second.empty())
                    loc.index.first = true ;
                break;
            }
            case dir_post:
            {
                std::cout<<"dirpost == "<<prevLoc.substr(indexStart, indexEnd - indexStart)<<std::endl;
                loc.dirpost.second = prevLoc.substr(indexStart, indexEnd - indexStart);
                std::cout<<"dirpost second == "<<loc.dirpost.second<<std::endl;
                if (!loc.index.second.empty())
                    loc.dirpost.first = true ;
                break;
            }
            case ret:
            {
                std::cout<<"ret == "<<prevLoc.substr(indexStart, indexEnd - indexStart)<<std::endl;
                loc.ret.second = prevLoc.substr(indexStart, indexEnd - indexStart);
                std::cout<<"ret second == "<<loc.ret.second<<std::endl;
                if (!loc.index.second.empty())
                    loc.ret.first = true ;
                break;
            }
            case alias:
            {
                std::cout<<"alias == "<<prevLoc.substr(indexStart, indexEnd - indexStart)<<std::endl;
                loc.alias.second = prevLoc.substr(indexStart, indexEnd - indexStart);
                std::cout<<"alias second == "<<loc.alias.second<<std::endl;
                if (!loc.index.second.empty())
                    loc.alias.first = true ;
                break;
            }
            case method:
            {
                const char *methods[] = {"DELETE", "POST", "GET", NULL};
                while (index < prevLoc.size() && prevLoc[index] != '}')
                {
                    for (int m = 0; methods[m] && index < prevLoc.size(); m++)
                    {
                        //std::cout<<"m == "<<m<<std::endl;
                        //std::cout<<"substr == "<<prevLoc.substr(index, strlen(methods[m]))<<std::endl;
                        if (prevLoc.compare(index, strlen(methods[m]), methods[m]) == 0)
                        {
                            switch(m)
                            {
                                case 0:
                                {
                                    loc.del = true;
                                    break;
                                }
                                case 1:
                                {
                                    loc.post = true;
                                    break;
                                }
                                case 2:
                                {
                                    loc.get = true;
                                    break;
                                }
                                default:
                                    break;
                            }
                        }
                    }
                    index++;
                }
            }
            default:
                break;
        }
    //     index = indexEnd + 1;
    //    // std::cout<<"index a la fin == "<<index<<std::endl;
        // index = prevLoc.find_first_of(";", indexEnd);
        index = indexEnd;
        if (index > prevLoc.size())
            break ;
        index += 1;
    }
    // std::cout<<"char == "<<prevLoc[index]<<std::endl;
    std::cout<<"path = "<<loc.path<<"\n get == " <<loc.get<<"\n post = " <<loc.post<<"\n del == "<<loc.del<<"\n cgi == "<< loc.cgi<<"\n autoindex == "<< loc.autoindex <<"\n index first == "<< loc.index.first<<"\n index second == "<< loc.index.second <<"\n dipost first == "<< loc.dirpost.first<<"\n dirpost second == "<< loc.dirpost.second <<"\n ret first == "<< loc.ret.first<< " ret second == "<<loc.ret.second<<" \nalias first ==  "<< loc.alias.first<<"alias second == "<< loc.alias.second<<"\n\n\n"<<std::endl;
    Location res(loc.path, loc.get, loc.post, loc.del, loc.cgi, loc.autoindex, loc.index, loc.dirpost, loc.ret, loc.alias);
    return (res);
}

std::string getLocation(size_t &index, const std::string &content, size_t &indexEnd)
{
    int level = 1;
    while(level && content[indexEnd])
    {
        if (content[indexEnd] == '{')
            level++;
        if (content[indexEnd] == '}')
            level--;
        indexEnd++;
    }
    if (index >= content.size())
        throw std::invalid_argument("location{...}");
    return (content.substr(index, indexEnd - index));
}

int addToServer(Data &servData, size_t &index, const std::string &content)
{
    const char  *serverInfo[] = {"server_name", "listen", "root", "body_size", "location", "error", NULL};
    size_t      indexEnd = 0, indexStart;
    size_t      i;
    int         c = 0;
    //if differend de server name cat sinon quiter si deja def
    while (index <= content.size() && content[index] != '}')
    {
        index = content.find_first_not_of(" \t", index);
        indexEnd = content.find_first_of(" \t", index);
        if (indexEnd > content.size() || index > content.size())
            break ;
        for (i = 0; serverInfo[i]; i++)
        {
            if (index > content.size())
                return (0) ;
            //std::cout<<"\ncompare : "<<content.substr(index,  strlen(serverInfo[i]))<<std::endl;
            if (content.compare(index, strlen(serverInfo[i]), serverInfo[i]) == 0)
            {
                indexStart = content.find_first_not_of(" \t", indexEnd);
                break ;
            }
        }
        if(i != error && i != location)
        {
            indexEnd = content.find_first_of(";", index);
            if (indexEnd > content.size() || indexStart > content.size())
                break ;
        }
        //std::cout<<"\ni == "<<i<<std::endl;
        switch(i)
        {
            case server_name:
            {
                if (servData.name.empty())
                {
                    servData.name = content.substr(indexStart, indexEnd - indexStart);
                   // std::cout<<"serv name = "<<servData.name<<std::endl;
                    break;
                }
                else
                    return (0);
            }
            case listen:
            {
                servData.listen += " " + content.substr(indexStart, indexEnd - indexStart);
               // std::cout<<"listen = "<<servData.listen<<std::endl;
                break;
            }
            case root:
            {
                servData.root += " " + content.substr(indexStart, indexEnd - indexStart);
               // std::cout<<"root = "<<servData.root<<std::endl;
                break;
            }
            case body_size:
            {
                if (servData.body.empty())
                {
                    servData.body = content.substr(indexStart, indexEnd - indexStart);
                   // std::cout<<"body = "<<servData.body<<std::endl;
                    break;
                }
                else
                    return (0);
            }
            case location:
            {
                index = content.find_first_not_of(" \t", indexEnd);
                if (index > content.size())
                   return (0);
                for (; content[indexEnd] && content[indexEnd] != '{'; indexEnd++)
                {
                   // std::cout<<"char == "<<content[indexEnd]<<std::endl;
                }
                indexEnd++;
                //std::cout<<<<std::end;
                //servData.loc.push_back(getLocation(index, content, indexEnd));
                //servData.locations.push_back(parseLocation(servData.loc[0]));
                servData.locations.push_back(parseLocation(getLocation(index, content, indexEnd)));
                break;
            }
            case error:
            {
                //std::cout<<"'''''''''''''''''''''''''''"<<std::endl;
                index = content.find_first_not_of(" \t", indexEnd);
                if (index > content.size())
                   return (0);
                for (; content[indexEnd] && content[indexEnd] != '{'; indexEnd++)
                {
                   // std::cout<<"char == "<<content[indexEnd]<<std::endl;
                }
                indexEnd++;
                servData.error.push_back(getLocation(index, content, indexEnd));
               // std::cout<<"err = "<<servData.error[0]<<std::endl;
                break;
            }
            default:
            {
                std::cout<<"////////////////////////////////////"<<std::endl;
                std::string errorMessage = "Invalid config file: ";
                errorMessage += std::string(content.c_str() + index, indexEnd - index);
                throw std::invalid_argument(errorMessage);
            }
        }
        //indexEnd = content.find_first_not_of(" \t", indexEnd);
       // if (indexEnd > content.size())
        //    return (0);
       // std::cout<<"char == "<<content[indexEnd]<<std::endl;
        if (indexEnd != std::string::npos && content[indexEnd] != '}')
            index = indexEnd + 1;
        else
            index = indexEnd;
        //std::cout<<"char == "<<content[index]<<std::endl;
    }
    index = content.find_first_not_of(" \t", index);
    if (index > content.size())
    {
        std::cout<<"char == "<<content[index]<<std::endl; 
        return (0);
    }
    //std::cout<<"char == "<<content[index]<<std::endl; 
    return (1);
}

bool getData(std::string &content, Data &servData, size_t &index)
{
    int status = 0;
    while (index <= content.size())
    {
       // std::cout<<"len == "<<content.size()<<" content == "<<content<<" index "<<index<<std::endl;
       // std::cout<<"la"<<std::endl;
        index = content.find_first_not_of("\n\t", index);
        //std::cout<<"la"<<std::endl;
        if (index > content.size())
            break ;
        //std::cout<<"comparing : "<<content.c_str() + index<<"\n\n"<<std::endl;
        if (content.compare(index, 6,"server") == 0)
        {
            index += 6;
            index = content.find_first_not_of(" \n\t", index);
            if (index > content.size())
                return (false);
//            std::cout<<"char = "<<content[index]<<std::endl;
            index++;
  //          std::cout<<"char = "<<content[index]<<std::endl;
            if (content[index - 1] == '{')
            {
                status = SERVER;
                break;    
            }
        }
        index++;
       // std::cout<<"index == "<<index<<std::endl;
    }
    while (index < content.size() && status == SERVER)
    {
        //std::cout<<"ici1"<<std::endl;
            //chercher les infos du serveur
        //if (status == LOCATION)
            //checher les infos de location //addToLocation(servData, index, content);
        //std::cout<<"ici2"<<std::endl;
        if (status == SERVER && !addToServer(servData, index, content))
        {
            std::cout<<"ici3"<<std::endl;
            return (false);
        }
        //std::cout<<content[index]<<std::endl;
        //std::cout<<"status == "<<status<<std::endl;
        if (content[index] == '}')
            status--;
        //std::cout<<"status == "<<status<<std::endl;
        if (status == OUTSIDE) 
            return (true);
        //std::cout<<"ici5"<<std::endl;
    }
    return (true);
}

void    clearData(Data &data)
{
    data.name.clear();
    data.root.clear();
    data.ports.clear();
    data.listen.clear();
    data.body.clear();
   // data.locations.clear();
    data.error.clear();
    data.loc.clear();
    data.bodymax = 0;
    data.good = true;
}

std::vector<Data> parsing(const char *filename)
{
    std::vector<Data>   serveurs;
    Data                servData;
    std::ifstream       file(filename);
    std::string         content, line;
    size_t              index = 0;

    clearData(servData);
    while (std::getline(file, line))
        content += line;
    while (index < content.size())
    {
        getData(content, servData, index);
        if (servData.name.empty())
            servData.good = false;
        if (servData.good)
        {
            double nb;
            char *endptr;
            for (const char *str = servData.listen.c_str(); *str; str++)
            {
                nb = strtod(str, &endptr);
                if ((!std::isspace(*str) && !std::isdigit(*str)) || nb == MAX(double)infinity() ||\
                    nb > MAX(int)max() || nb < 0 || nb != std::floor(nb))
                {
                    //std::cout<<"str == "<<str<<" nb == "<<nb<<std::endl;
                    std::string errorMessage = "Invalid config file: ";
                    errorMessage += servData.listen;
                    throw std::invalid_argument(errorMessage);
                }
                else
                    servData.ports.push_back(nb);
                str = endptr;
                if (!*endptr)
                    break ;
                //std::cout<<"str == "<<str<<" nb == "<<nb<<std::endl;
            }
               // std::cout<<"data nsme = "<<servData.name<<std::endl;
                serveurs.push_back(servData);
        }
        clearData(servData);
    }
    return (serveurs);
}

int main()
{
    try
    {
        std::vector<Data> t = parsing("testconfig");
        if (t.empty())
            std::cout<<"youhou"<<std::endl;
        else
        {
            for (int i = 0; i<t.size();i++)
            {
                std::cout<<"server["<<i<<"] name "<<t[i].name<<std::endl;
                for(int j = 0; j < t[i].ports.size(); j++)
                    std::cout<<"server["<<i<<"] port["<<j<<"] "<<t[i].ports[j]<<", "<<std::endl;
                std::cout<<"server["<<i<<"] root "<<t[i].root<<std::endl;
                std::cout<<"server["<<i<<"] body "<<t[i].body<<std::endl;
                for(int j = 0; j < t[i].loc.size(); j++)
                    std::cout<<"server["<<i<<"] location["<<j<<"] "<<t[i].loc[j]<<std::endl;
                for(int j = 0; j < t[i].error.size(); j++)
                    std::cout<<"server["<<i<<"] error["<<j<<"] "<<t[i].error[j]<<std::endl;
            }
        }
    }
    catch(std::exception &e)
    {
        std::cout<<e.what()<<std::endl;
    }
}