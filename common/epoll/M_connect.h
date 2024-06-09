#include "head.h"
#include "mystruct.h"
#define buffsize 1024
void error_handling(char *message)
{
	perror(message);
	exit(1);
}
class myconnect{
    public:
    struct sockaddr_in service_addr;
    struct in_addr go_ip;
    u_int16_t go_port;
    char buffer[buffsize];
    int sockfd;
    myconnect(string go_ip_s,int go_port_s){
        go_ip.s_addr=inet_addr((char*)go_ip_s.c_str());
        cout<<go_port_s<<endl;
        go_port=htons(go_port_s);
    }
    int init(){
        memset(&service_addr,0,sizeof(service_addr));
        service_addr.sin_family=AF_INET;
        service_addr.sin_port=go_port;
        service_addr.sin_addr.s_addr=go_ip.s_addr;
        return sockfd=socket(AF_INET,SOCK_STREAM,0);
    }
    int run(){
        service_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
        if(connect(sockfd,(struct sockaddr*)&service_addr,sizeof(service_addr))<0){
            error_handling("connect() error!");
            return -1;
        }
        return 1;
    }
    int myread(){
        int num=0;
        string res;
        cout<<"处理中"<<endl;
        std::cout<<num<<endl;
        num=read(sockfd,buffer,buffsize-1);
        std::cout<<num<<endl;
        while(num>0){
            for(int i=0;i<num;i++){
                if(buffer[i]=='#'){
                    cout<<res<<endl;
                    return 1;
                }
                res+=buffer[i];
            }
            num=read(sockfd,buffer,buffsize-1);
            cout<<num<<endl;
        }
        cout<<res<<endl;
        return 1;
    }
    int mywrite(string Request){
        int error=0;
        socklen_t len =sizeof(error);
        if(getsockopt(sockfd,SOL_SOCKET,SO_ERROR,&error,&len)==0){
            if(errno!=0){
                cout<<"未连接，尝试重新连接..."<<endl;
                if(run()<0){
                    cout<<"连接失败"<<endl;
                    return -1;
                }
            }
        }
        int length=Request.size();
        char buf[length+2];
        for(int i=0;i<length;i++){
            buf[i]=Request[i];
        }
        buf[length]='#';
        buf[length+1]='\0';
        if(send(sockfd,buf,length+1,0)<0){
            cout<<"发送请求失败"<<endl;
            return -1;
        }else{
            cout<<"处理中"<<endl;
        }
        return 1;
    }

    ~myconnect(){
        close(sockfd);
    }
};