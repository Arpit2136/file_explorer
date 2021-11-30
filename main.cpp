#include<sys/ioctl.h>
#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <bits/stdc++.h>
#include <stdint.h>
#include<unistd.h>
#include<bits/stdc++.h>
#include<termios.h>
#include<vector>
#include<fcntl.h>
#include<fstream>
#include<iostream>
#include<string>
#include<stdlib.h>
// #include <sys/wait.h>
// #include<stdarg.h>



using namespace std;


vector<string> list_of_dir;

stack<string> current_path_till_now;

stack<string> backward_path;



struct winsize w;
int current_window_row_size;
int cursor=0;
string root_directory;
string root_directory_parent;
int current_display_size=0;
int upper_limit=0;
int lower_limit=0;
int key1=' ';
int key2=' ';
int key3=' ';

void open_file_directory(string s);
int getch();
void display_overflows_for_k(int a,int b);
void display_overflows_for_l(int a,int b);
void open_command_mode();
void display_home(string s);
void display_next_directory(int a);
void display_previous_directory();


void open_normal_mode()
{
	while(current_path_till_now.empty()==false)
		current_path_till_now.pop();
	cout<<"\033[H\033[J";
	// cout<<"Welcome to normal mode";
	open_file_directory(root_directory);
	int key1=' ';
	int key2=' ';
	int key3=' ';
	while(1)
	{
		key1=getch();
		if(key1==27)
		{
			key2=getch();
			key3=getch();

		}

		//pressing key 'k'
		if(key1==107)
		{
			if(lower_limit==0)
			{
				continue;
			}
			else
			{
				upper_limit--;
				lower_limit=upper_limit-current_window_row_size;
				
				display_overflows_for_k(lower_limit,upper_limit);	
			}
			
		}

		//pressing key 'l'
		if(key1==108)
		{
			if(upper_limit==list_of_dir.size())
			{
				continue;
			}
			else
			{
				lower_limit++;
				upper_limit=lower_limit+current_window_row_size;
				
				display_overflows_for_l(lower_limit,upper_limit);	
			}
			
		}
		if(key1==58)
		{
			open_command_mode();
		}


		//pressing backspace
		if(key1==127)
		{
			// up_one_level();
			open_file_directory("..");
      // cursor=1;
		}

		// for home on pressing h
		if(key1==104)
		{
			display_home(root_directory);
		}
		//enter key
		if(key1==10)
		{
			// if(check_directory_for_all_types(list_of_dir[cursor]))
			// {
			if(list_of_dir[cursor]==".")
			{
				continue;
			}
			// if(list_of_dir[cursor]=="..")
			else 
			{
				// open_back_directory();
			// }
			// else
			// {
				open_file_directory(list_of_dir[cursor]);
			}
			
			// }
			

		}
		if(key1==27 && key2==91)
		{
			//uparrow
			if(key3==65)
			{

				cursor--;
				if(cursor<0)
				{
					cursor=0;
				}
				cout<<"\u001b[1A";
            // }

			}
			//down arrow
			else if(key3==66)
			{
				cursor++;
				if(cursor<=list_of_dir.size())
				{
					cout<<"\u001b[1B";
				}
				else
				{
					cursor=list_of_dir.size();
				}
				
			}
			// right arrow key
			else if(key3==67)
			{
				display_next_directory(cursor);
			}
			//left arrow key
			else if(key3==68)
			{
				display_previous_directory();
			}




		}
	}


}

void display_previous_directory()
{
	if(current_path_till_now.top()==root_directory)
		return;
	else
	{
		string s=current_path_till_now.top();
		int ss=s.size();
		current_path_till_now.pop();
		if(current_path_till_now.empty()==true)
		{
			open_file_directory(root_directory);
			return;
		}
		else
		{
			string p="";
			int i;
			i=ss-1;
			while(s[i]!='/')
			{
				p=s[i]+p;
				i--;
			}
			open_file_directory(p);
		}
		
	}

}
int check_directory( string path)
{
	// cout<<"hlo\n";
	
	struct stat check_dir;
	if (stat(&path[0], &check_dir) != 0)
		return 0;
	int s=path.length();

	if(path[s-1]=='.')
		return 0;
	// cout<<path<<"\n";
	return S_ISDIR(check_dir.st_mode);
}
void copy_fun(vector<string> &copy_inp,string current_path_of_files)
{
	int size=copy_inp.size();
	int i;
	string line;
	string destination_directory=copy_inp[size-1];
	string des=destination_directory;
	string source;



	for(i=1;i<size-1;i++)
	{
    // char *d,*s;
		des=destination_directory;
		source="";
		des="/"+des+"/"+copy_inp[i];
		source=current_path_of_files+"/"+copy_inp[i];
		// cout<<"des "<<des<<"\n";
		// cout<<"source "<<source<<"\n";
		ofstream f1;
		fstream f2;

		f1.open(des);
		f2.open(source);
		if(f1.is_open())
		{
			// cout<<"success";
		}
		else
		{
			cout<<"fail";
			return;
		}
		while(f2)
		{

			getline(f2,line);
			// cout<<line;
			f1<<line<<"\n";
		}
		// struct stat info;

		// fchown()

		f1.close();
		f2.close();

	}


}


void move_fun(vector<string> &copy_inp,string current_path_of_files)
{
	copy_fun(copy_inp,current_path_of_files);
	int i;
	int size=copy_inp.size();
	for(i=1;i<size-1;i++)
	{
		remove(&copy_inp[i][0]);
	}
}


void rename_fun(vector<string> &copy_inp,string current_path_of_files)
{

	string line;
  // string destination_directory=copy_inp[size-1];
	string des="";
	string source="";
	des="";
	source="";

	des=current_path_of_files+"/"+copy_inp[2];
	source=current_path_of_files+"/"+copy_inp[1];
	// cout<<"source "<<source<<"\n";
	// cout<<"des "<<des<<"\n";

	ofstream f1;
	fstream f2;

	f1.open(des);
	f2.open(source);
	if(f1.is_open())
	{
		// cout<<"success";
	}
	else
	{
		cout<<"fail";
		return;
	}
	while(f2)
	{

		getline(f2,line);
		// cout<<line;
		f1<<line<<"\n";
	}


	f1.close();
	f2.close();
	remove(&source[0]);

}

void create_dir(vector<string> &copy_inp)
{
	string des_path="/"+copy_inp[2];
	string dir_name=des_path+"/"+copy_inp[1]+"/";
	mkdir(&dir_name[0],0777);
}
void create_file(vector<string> &copy_inp)
{
	ofstream f1;
	string file_name="/"+copy_inp[2]+"/"+copy_inp[1];
	f1.open(file_name);
	f1.close();

}

void delete_file(vector<string> &copy_inp,string current_path_of_files)
{
	string path_of_file="/"+current_path_of_files+"/"+copy_inp[2]+"/"+copy_inp[1];
	remove(&path_of_file[0]);
}

void goto_fun(vector<string> &copy_inp)
{
	string s=copy_inp[1];
	int rc = chdir(&s[0]);
    if (rc < 0) {
        // TODO: handle error. use errno to determine problem
    }
	current_path_till_now.push(&s[0]);
}



int check_directory_for_delete( string path)
{
	// cout<<"hlo\n";
	
	struct stat check_dir;
	if (stat(&path[0], &check_dir) != 0)
		return 0;
	return S_ISDIR(check_dir.st_mode);
}

void delete_dir(vector<string> &copy_inp,string current_path_of_files)
{

	queue<string> delete_list;
	map<string,int> visited;
	string path=current_path_of_files+"/"+copy_inp[2]+copy_inp[1];
	cout<<path<<"\n";
	delete_list.push(path);
	visited[path]++;
	char *c=&path[0];
	// if(rmdir(c))
	// {
	// 	cout<<strerror(errno) ;
	// }
	if(rmdir(c)==0)
	{
		rmdir(c);
		// cout<<"hello\n";
	}
	while(delete_list.empty()==false)
	{
		string p=delete_list.front();

		delete_list.pop();
		if(rmdir(&p[0])==-1)
		{
			DIR *d;
			struct dirent *e;
			struct stat st;
			d=opendir(&p[0]);
			while((e=readdir(d))!=NULL)
			{


// cout<<p+"/"+e->d_name<<"\n";
				stat(e->d_name,&st);

				
				if(check_directory(p+"/"+e->d_name)==1)
				{

					cout<<p+"/"+e->d_name<<"\n";
					if(visited[p+"/"+e->d_name]<1)
					{
						delete_list.push(p+"/"+e->d_name);
						visited[p+"/"+e->d_name]++;
					}

					


				}
				else
				{
					remove((&e->d_name)[0]);
					
				}


			}
		}
	}

}

void goto_search(vector<string> &copy_fun,string current_path_of_files)
{
	queue<string> all_sub_directories;
	
  // cout<<current_path_of_files;
	all_sub_directories.push(current_path_of_files);
	
	while(all_sub_directories.empty()==false)
	{
		string p=all_sub_directories.front();
		
		all_sub_directories.pop();

		DIR *d;
		struct dirent *e;
		struct stat st;
		d=opendir(&p[0]);
		while((e=readdir(d))!=NULL)
		{



			stat(e->d_name,&st);
			
			if(copy_fun[1]==e->d_name)
			{
				// cout<<"True";

				return;
			}
			if(check_directory(p+"/"+e->d_name)==1)
			{

// cout<<p+"/"+e->d_name<<"\n";
				
				all_sub_directories.push(p+"/"+e->d_name);
				

			}


		}
	}
	// cout<<"False";
	return;
}


void open_command_mode()
{
	int c=list_of_dir.size();
	while(cursor!=c+1)
	{
		cout<<"\u001b[1B";
		cursor++;
	}
// cout<<"\033[cB";

	cout<<"\n";
	cout<<"Opened in command mode";
	cout<<"\n";
	

	string inp_cmd;
	while(1)
	{


		getline(cin,inp_cmd);
		char *inp=&inp_cmd[0];
		int j;
		vector<string> splitted_command;
		char *i;

		i=strtok(inp," ");
		while(i!=NULL)
		{
			splitted_command.push_back(i);
			i=strtok(NULL," ");
		}
  // for(j=0;j<splitted_command.size();j++)
  // {
  //   cout<<splitted_command[j]<<"\n";
  // }


		if(splitted_command[0]=="copy")
		{
			copy_fun(splitted_command,current_path_till_now.top());
		}
		else if(splitted_command[0]=="move")
		{
			move_fun(splitted_command,current_path_till_now.top());

		}
		else if(splitted_command[0]=="rename")
		{
			rename_fun(splitted_command,current_path_till_now.top());

		}
		else if(splitted_command[0]=="create_file")
		{
			create_file(splitted_command);
		}
		else if(splitted_command[0]=="create_dir")
		{
			create_dir(splitted_command);
		}
		else if(splitted_command[0]=="delete_file")
		{
			delete_file(splitted_command,current_path_till_now.top());
		}

		else if(splitted_command[0]=="delete_dir")
		{
			delete_dir(splitted_command,current_path_till_now.top());
		}
		else if(splitted_command[0]=="goto")
		{
			goto_fun(splitted_command);
		}
		else if(splitted_command[0]=="search")
		{
			goto_search(splitted_command,current_path_till_now.top());
		}
		else if(splitted_command[0]=="\033")
		{
			open_normal_mode();
		}
		else
		{
			continue;
		}

	}

}







// void open_command_mode()
// {

// }

void get_window_size()
{
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	current_window_row_size=w.ws_row;
}


void new_display(int starting_index)
{
	cout<<"\033c";
	for(int i=cursor+1;i<=current_window_row_size;i++)
	{
		cout<<list_of_dir[i]<<"\n";
		cursor++;
		cout<<"\u001b[1A";
	}
  // cout<<"\u001b[1A";

}





int check_directory_for_all_types( string path)
{
	// cout<<"hlo\n";
	// cout<<path<<"\n";
	struct stat check_dir;
	if (stat(&path[0], &check_dir) != 0)
		return 0;
	
	
	return S_ISDIR(check_dir.st_mode);
}
string convertToString(char* a) 
{ 
	int i; 
	string s = a; 
	
	return s; 
} 

void open_file_directory(string s)
{
	char cwd[PATH_MAX];
	char *c;
	struct tm *foo;
	struct group *grp;
	struct passwd *pwd;
	c=getcwd(cwd,PATH_MAX);
	
	string p=convertToString(c);
	
	if(s==".." && current_path_till_now.top()==root_directory)
		return;
	struct stat st;
	string file_open;

	if(current_path_till_now.empty()==false)
	{
		file_open=current_path_till_now.top()+"/"+s;
		// current_path_till_now.pop();
	}
	else
	{	//empty stack
		file_open=s;
		if(s=="..")
			return;

	}
	
	int i;
	
	DIR *d;
	struct dirent *e;
	struct stat sb;
	if(stat(&file_open[0], &sb) == 0 && S_ISREG(sb.st_mode))
	{
		pid_t pid=fork();
		// cout<<file_open<<"\n";
		if(pid==0)
		{
			execl("/usr/bin/xdg-open","xdg-open",&file_open[0],NULL);
			// exit(1);
		}
		return;
	}
	d=opendir(&file_open[0]);

	
	if(d==NULL)
	{
		// struct stat sb;
		// if(stat(&file_open[0], &sb) == 0 && S_ISREG(sb.st_mode))
		// {
		// 	pid_t pid=fork();
		// // cout<<file_open<<"\n";
		// 	if(pid==0)
		// 	{
		// 		execl("/usr/bin/xdg-open","xdg-open",&file_open[0],NULL);
		// 	// exit(1);
		// 	}
		// }
		
		// to open file in editor
		
		return;
	}
	else
	{
		cursor=0;
		list_of_dir.clear();
		cout<<"\033[H\033[J";
		
	}
	
	if(s=="..")
		current_path_till_now.pop();
	else
		current_path_till_now.push(file_open);

	while((e=readdir(d))!=NULL)
	{

		cursor++;

		stat(e->d_name,&st);

		cout<<std::left << std::setw(35) << e->d_name << std::setw(25) << st.st_size;
		// cout<<<<" ";
		// cout << setw(25) ;
		cout<<((st.st_mode & S_IRUSR)? "r":"-");
		cout<<((st.st_mode & S_IWUSR)? "w":"-");
		cout<<((st.st_mode & S_IXUSR)? "x":"-");
		cout<<("  ");
		// cout << setw(25) ;
  // cout<<("File Permissions Group ");
		cout<<((st.st_mode & S_IRGRP)? "r":"-");
		cout<<((st.st_mode & S_IWGRP)? "w":"-");
		cout<<((st.st_mode & S_IXGRP)? "x":"-");
		cout<<("  ");
		// cout << setw(25) ;
  // cout<<("File Permissions Other ");
		cout<<((st.st_mode & S_IROTH)? "r":"-");
		cout<<((st.st_mode & S_IWOTH)? "w":"-");
		cout<<((st.st_mode & S_IXOTH)? "x":"-");
		cout<<("   ");
		foo = gmtime(&(st.st_mtime));
	// cout<<(foo->tm_year)<<"/";
		cout<<(foo->tm_mon)<<"/";
		cout<<(foo->tm_mday)<<"-";
		cout<<(foo->tm_hour)<<":";
		cout<<( foo->tm_min);
		// cout<<(ctime(&st.st_mtime));
		string name=convertToString(e->d_name);
		list_of_dir.push_back(e->d_name);
		cout<<"\n ";
	

	}
	
	current_display_size=list_of_dir.size()%current_window_row_size;
	upper_limit=list_of_dir.size();

	lower_limit=upper_limit-current_display_size;

	


	



}
void open_back_directory()
{

	char cwd[PATH_MAX];
	char *c;

	c=getcwd(cwd,PATH_MAX);
	
	string p=convertToString(c);
	// backward_path.push(current_path_till_now.top());
	// current_path_till_now.pop();
	// cout<<"hello\n";
	string s=p;
	s=s+"/..";
	backward_path.push(s);
	int i;
	
	DIR *d;
	struct dirent *e;
	d=opendir(&s[0]);
	struct stat st;
	
	if(d==NULL)
		return;
	else
	{
		cursor=0;
		list_of_dir.clear();
		cout<<"\033[H\033[J";
		// current_path_till_now.push(&file_open[0]);
	}
	while((e=readdir(d))!=NULL)
	{

		cursor++;

		stat(e->d_name,&st);
		list_of_dir.push_back(e->d_name);
		cout<<e->d_name<<"\n ";

	}
	
	current_display_size=list_of_dir.size()%current_window_row_size;
	upper_limit=list_of_dir.size();

	lower_limit=upper_limit-current_display_size;

}
void up_one_level()
{
	
	string s=current_path_till_now.top();
	current_path_till_now.pop();
	string p="";
	int size_dir_name=s.size();
	int i;
	i=size_dir_name-1;
	while(s[i]!='/')
	{
		p=s[i]+p;
		i--;
	}
	open_file_directory(p);


}

void display_last_visited_directory()
{



}


void display_home(string s)
{
	while(current_path_till_now.empty()==false)
	{
		backward_path.push(current_path_till_now.top());
		current_path_till_now.pop();
	}

	open_file_directory(s);

}

void display_next_directory(int k)
{
	DIR *d;
	// struct dirent *e;
	
	string s=current_path_till_now.top();
	string p;
	k++;
	while(k<list_of_dir.size())
	{
		p=s+"/"+list_of_dir[k];
		d=opendir(&p[0]);
		if(d==NULL || list_of_dir[k]==".."||list_of_dir[k]==".")
		{
			k++;
			continue;
		}
		else
		{
			open_file_directory(list_of_dir[k]);
			return;
		}
		// k++;
		

	}
	// return;
}



void display_overflows_for_k(int l,int u)
{
	cout<<"\033[H\033[J";
	// cout<<"\033[H";
	cursor=l;

	int i;
	for(i=l;i<u;i++)
	{
		// cout<<"\u001b[1A";
		cursor++;
		cout<<list_of_dir[i]<<"\n";
	}

}

void display_overflows_for_l(int l,int u)
{
	cout<<"\033[H\033[J";
	// cout<<"\033[H";
	cursor=l;
	// cursor=current_window_row_size;
	int i;
	for(i=l;i<u;i++)
	{
		// cursor++;
		// cout<<"\u001b[1B";
		cursor++;
		cout<<list_of_dir[i]<<"\n";
	}

}

int getch(void)
{
	int ch;
	struct termios oldt;
	struct termios newt;
	tcgetattr(STDIN_FILENO, &oldt); 
	newt = oldt;  
	newt.c_lflag &= ~(ICANON | ECHO); 
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar(); 
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt); 
	return ch; 
}
int main() {
	// cout<<"\033[H\033[J";
	// DIR *d;
	// struct dirent *e;
	// current_path_till_now.clear();
	get_window_size();
	// cout<<"hello";
	char cwd[PATH_MAX];
	char *c;

	c=getcwd(cwd,PATH_MAX);
	
	string s=convertToString(c);
	root_directory=s;
	root_directory_parent=root_directory+"/..";
	
	

	
	open_normal_mode();
	
	return 0;
}
