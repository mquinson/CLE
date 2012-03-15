#include "teststrace.h"

action *allocate_action(){
	action *a = malloc(sizeof(action));
	a->begin =1;
	a->end=0;
	a->wait=0;
	a->pid_father =0;
	a->pid_son=0;
	a->call = malloc(50*sizeof(char));
	return a;
}

void free_action(action *a){
	free(a->call);
	free(a);
}

action *build_again_action(char *line){
	action *a = allocate_action();
	int i=0,size = strlen(line),sizec=0;
	char *data = malloc((size+1)*sizeof(char));
	
	/**Read begin*/
	while(i<size && line[i]!='/'){
		data[i]=line[i];
		i++;
	}
	data[i]='\0';
	a->begin = atoi(data);
	sizec=i+1;
	i=0;
	/**Read pid_father*/
	while(i<size-sizec && line[sizec+i]!='/'){
		data[i]=line[sizec+i];
		i++;
	}
	data[i]='\0';
	a->pid_father=atoi(data);
	sizec+=i+1;
	i=0;
	/**Read call*/
	while(i<size-sizec && line[sizec+i]!='/'){
		data[i]=line[sizec+i];
		i++;
	}
	data[i]='\0';
	strcpy(a->call,data);
	sizec+=i+1;
	i=0;
	/**Read wait*/
	while(i<size-sizec && line[sizec+i]!='/'){
		data[i]=line[sizec+i];
		i++;
	}
	data[i]='\0';
	a->wait=atoi(data);
	sizec+=i+1;
	i=0;
	/**Read pid_son*/
	while(i<size-sizec && line[sizec+i]!='/'){
		data[i]=line[sizec+i];
		i++;
	}
	data[i]='\0';
	a->pid_son=atoi(data);
	sizec+=i+1;
	i=0;
	/**Read end*/
	while(i<size-sizec && line[sizec+i]!='\0' && line[sizec+i]!='\n'){
		data[i]=line[sizec+i];
		i++;
	}
	data[i]='\0';
	a->end=atoi(data);
	
	free(data);
	return a;
}

list_lines *allocate_list_lines(){
	list_lines *l = malloc(sizeof(list_lines));
	l->lines = malloc(50*sizeof(char *));
	l->size=0;
	return l;
}

void free_list_lines(list_lines *l){
	free(l->lines);
	free(l);
}

void add_line(list_lines *l,char* line){
	l->lines[l->size]=malloc((strlen(line)+1)*sizeof(char));
	strcpy(l->lines[l->size],line);
	l->size++;
}

void delete_line(list_lines *l, int pos){
	free(l->lines[pos]);
	l->size--;
}

action *parser(char *line){
	action *a = allocate_action();
	int i=0,size = strlen(line),sizec=0;
	char *num_pid_f = malloc((size+1)*sizeof(char));
	char *action = malloc((size+1)*sizeof(char));
	char *num_pid_s = malloc((size+1)*sizeof(char));
	int resumed=0;
	
	/**Read pid father*/
	while(i<size && line[i]!=' '){
		num_pid_f[i]=line[i];
		i++;
	}
	num_pid_f[i]='\0';
	a->pid_father = atoi(num_pid_f);
	sizec=i;
	while(line[sizec]==' ' && sizec<size)
		sizec++;
	i=0;
	/**Read call or resumed*/
	while(i<size-sizec && line[sizec+i]!='('){
		if(!resumed && line[sizec+i]=='<'){
			resumed = 1;
			a->begin =0;
			sizec+=4;
		}
		else if(resumed && line[sizec+i]=='>'){
			break;
		}
		else if(!resumed){
			action[i]=line[sizec+i];
		}
		else{
			if(line[sizec+i]==' ')
				break;
			else
				action[i-1]=line[sizec+i];
		}
		i++;
	}
	//printf("%s\n",action);
	if(resumed)
		action[i-1]='\0';
	else
		action[i]='\0';
	if(!strcmp(action,"--- SIGCHLD ")){
		strcpy(a->call,"SIGCHLD");
		a->end =1;
		free(num_pid_f);
		free(action);
		//free(num_pid_s);
		return a;
	}
	strcpy(a->call,action);
	sizec+=++i;
	i=0;
	/**Read unfinished or not*/
	while(i+1<size-sizec && (line[sizec+i]!='=' || line[sizec+i+1]!=' ')){
		if(line[sizec+i]=='<'){
			a->wait = 1;
			free(num_pid_f);
			free(action);
			//free(num_pid_s);
			return a;
		}
		i++;
	}
	sizec+=i+2;
	i=0;
	/**Read pid son*/
	while(i<size-sizec && line[sizec+i]!='\0' && line[sizec+i]!=' ' && line[sizec+i]!='\n'){
		num_pid_s[i]=line[sizec+i];
		i++;
	}
	num_pid_s[i]='\0';
	//printf("pid_s : %s %d\n",num_pid_s,i);
	if(strcmp("?",num_pid_s))
		a->pid_son = atoi(num_pid_s);
	a->end =1;
	free(num_pid_f);
	free(action);
	//free(num_pid_s);
	return a;
}

void writing(int fds,char* name_prog){
	if(!fork()){
  		creat("res/tmp.txt",0666);
  		int fdout = open("res/tmp.txt",O_WRONLY);
  		dup2(fdout,1);
    	execlp("strace","strace","-f","-e",TRACE_FORK,"-o","res/lala",name_prog, NULL);
  	}
  	else{
    	FILE* buffer = fopen("res/lala", "r");
    	char* buf=NULL;
    	char *line = malloc(500*sizeof(char));
    	action *action;
    	size_t len=0;
    	int got = 0;
    	creat("res/resultat.txt",0666);
    	creat("res/CLE1.txt",0666);
    	int fd = open("res/resultat.txt",O_WRONLY);
    	int fd1 = open("res/CLE1.txt",O_WRONLY);
    	while((got=getline(&buf, &len, buffer))!=-1){
      		//printf("write : %d %s",got,buf);
      		write(fd,buf,got);
      		action = parser(buf);
	  		//printf("read : %d %s\n",got,list[i]);
	  		sprintf(line,"%d/%d/%s/%d/%d/%d\n",action->begin,action->pid_father,action->call,action->wait,action->pid_son,action->end);
	  		//printf("%s",line);
	  		write(fd1,line,strlen(line));
	  		write(fds,line,strlen(line));
	  		free_action(action);
    	}
    	free(line);
    	free(buf);
    	close(fd);
  	}
  	wait(NULL);
}

int readline (int fd, char *line, int maxlen){
	int n, rc, retvalue, encore=1; 
	char c, *tmpptr=line; 
	for (n=1; (n < maxlen) && (encore) ; n++) {
		if ( (rc = read (fd, &c, 1)) ==1) {
			*tmpptr++ =c; 
			if (c == '\n'){  /* fin de ligne atteinte */
				encore =0; 
				retvalue = n;
			}
		}
		else if (rc ==0) {  /* plus rien à lire */
			encore = 0;
			if (n==1) 
				retvalue = 0;  /* rien a été lu */
			else 
				retvalue = n;
		}
		else { /*rc <0 */
			
		}
	}
	*tmpptr = '\0';  /* pour terminer la ligne */
	return (retvalue);
}
/*
list_lines *extract_lines(char *lines){
	//printf("send : %s\n",lines);
	int size=strlen(lines);
	int pos = 0,posc=0;
	list_lines *list_lines = allocate_list_lines();
	while(pos<size){
		char *line = malloc((size+1)*sizeof(char));
		posc=pos;
		while(pos<size && lines[pos]!='\n'){
			line[pos-posc]=lines[pos];
			pos++;
		}
		line[pos-posc]='\0';
		//printf("line number %d : %s\n",list_lines->size,line);
		add_line(list_lines,line);
		pos++;
	}
	return list_lines;
}*/

int isInListing(int *listing,int size,int pid_f){
	int i;
	for(i=0;i<size;i++){
		if(listing[i]==pid_f)
			return i;
	}
	return -1;
}

void clear_listing(int *listing,int size){
	int i;
	for(i=0;i<size;i++)
		listing[i]=0;
}

void read_info(int fdl,int fdw){
	char *buf =malloc(501*sizeof(char));
	char line[500];
	int *listing = malloc(50*sizeof(int));
	int *bool_listing = malloc(50*sizeof(int));
	int size_listing=0,num_turn=0,pos=-1;
	creat("res/CLE.txt",0666);
	int got=0,fd = open("res/CLE.txt", O_WRONLY);
	//printf("File open\n");
	action *action;
	clear_listing(listing,50);
	clear_listing(bool_listing,50);
	do{
		if ((got = readline(fdl, buf, 500)) < 0){
            //perror("parent - read");
      		got=1;
        }
		else if(got>0){
			//printf("size : %d\n",got);
			buf[got]='\0';
			//printf("%s",buf);
			//list_lines *list=extract_lines(buf);
			/*int nb=list->size,i;*/
			/*for(i=0;i<nb;i++){*/
				/*Build action and manage the turns*/
				action = build_again_action(buf/*list->lines[i]*/);
				if((pos=isInListing(listing,size_listing,action->pid_father))==-1){
					listing[size_listing]=action->pid_father;
					size_listing++;
					bool_listing[size_listing-1]=1;
					//printf("new proc call : %d\n",size_listing-1);
				}
				else{
					if(bool_listing[pos]){
						clear_listing(bool_listing,size_listing);
						bool_listing[pos]=1;
						num_turn++;
						char line2[50];
						sprintf(line2,"new turn\n");
						//printf("%s",line2);
						write(fdw,line2,strlen(line2));
						write(fd,line2,strlen(line2));
					}	
					else{
						bool_listing[pos]=1;
						//printf("proc %d call\n",pos);
					}
				}
				/*Print result*/
				/*sprintf(line,"%d : %s : %d",action->pid_father,action->call,action->pid_son);
				printf("%s",line);*/
				sprintf(line,"%d/%d/%s/%d/%d/%d\n",action->begin,action->pid_father,action->call,action->wait,action->pid_son,action->end);
				if((action->end && (strcmp(action->call,"clone") || action->pid_son!=0))|| !strcmp("wait4",action->call) || !strcmp("waitpid",action->call)){
					write(fdw,line,strlen(line));
				}
				write(fd,line,strlen(line));
				free_action(action);
				/*delete_line(list,i);*/
			/*}*/
			/*free_list_lines(list);*/
		}
	}
	while(got>0);
	free(bool_listing);
	free(buf);
	//free(fd);
	free(listing);
}

void execute_proc(char *name_binary,int fd){
	/*Tester si le fifo existe et creer le fifo si necessaire*/
	mkdir("res",01777);
	mkfifo("res/lala",0666);
	int fde[2];
	pipe(fde);
	if(!fork()){
		close(fde[0]);
		writing(fde[1],name_binary);
		close(fde[1]);
	}
	else{
		close(fde[1]);
		read_info(fde[0],fd);
		close(fde[0]);
	}
}

int main(){
  char *line = malloc(500*sizeof(char));
  action *action = parser("2962  <... clone resumed> child_stack=0, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0xb7571938) = 2965 \n");
  sprintf(line,"%d/%d/%s/%d/%d/%d\n",action->begin,action->pid_father,action->call,action->wait,action->pid_son,action->end);
  printf("%s",line);
  return 0;
}
