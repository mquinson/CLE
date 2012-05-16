#include "teststrace.h"
#include "../UI/CLE.h"
#include "exercise.h"


action *allocate_action(){
	action *a = malloc(sizeof(action));
	a->begin =1;
	a->end=0;
	a->wait=0;
	a->pid_father =0;
	a->pid_son=0;
	a->call = malloc(50*sizeof(char));
	a->call[0] = '\0';
	a->fd=0;
	a->message = malloc(MAX_LINE*sizeof(char));
	a->message[0]='\0';
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
	while(i<size-sizec && line[sizec+i]!='/'){
		data[i]=line[sizec+i];
		i++;
	}
	data[i]='\0';
	a->end=atoi(data);
	sizec+=i+1;
	i=0;
	
	/**Read fd*/
	while(i<size-sizec && ((i>1 && line[sizec+i-1]!='"') || i<=1 ) && line[sizec+i]!='/' ){
		data[i]=line[sizec+i];
		i++;
	}
	data[i]='\0';
	a->fd=atoi(data);
	sizec+=i+1;
	i=0;
	
	/**Read message*/
	while(i<size-sizec && line[sizec+i]!='\0' && line[sizec+i]!='\n'){
		data[i]=line[sizec+i];
		i++;
	}
	data[i]='\0';
	strcpy(a->message,data);
	
	free(data);
	return a;
}

action *parser_command(char *line){
	action *a = allocate_action();
	int i=0,size = strlen(line),sizec=0;
	char *data = malloc((size+1) * sizeof(char));
	int resumed=0;
	
	/**Read pid father*/
	while(i<size && line[i]!=' '){
		data[i]=line[i];
		i++;
	}
	data[i]='\0';
	a->pid_father = atoi(data);
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
			data[i]=line[sizec+i];
		}
		else{
			if(line[sizec+i]==' ')
				break;
			else
				data[i-1]=line[sizec+i];
		}
		i++;
	}
	
	if(resumed)
		data[i-1]='\0';
	else
		data[i]='\0';
	if(!strcmp(data,"--- SIGCHLD ")){
		strcpy(a->call,"SIGCHLD");
		a->end =1;
		free(data);
		return a;
	}
	else if(!strcmp(data,"--- SIGSEGV ")){
		strcpy(a->call,"SIGSEGV");
		a->end =1;
		free(data);
		return a;
	}
	else if(!strcmp(data,"+++ killed by SIGSEGV ")){
		strcpy(a->call,"killed by SIGSEGV");
		a->end =1;
		free(data);
		return a;
	}
	else if(!strcmp(data,"--- SIGABRT ")){
		strcpy(a->call,"SIGABRT");
		a->end =1;
		free(data);
		return a;
	}
	else if(!strcmp(data,"+++ killed by SIGABRT ")){
		strcpy(a->call,"killed by SIGABRT");
		a->end =1;
		free(data);
		return a;
	}
	else if(!strcmp(data,"--- SIGIOT ")){
		strcpy(a->call,"SIGIOT");
		a->end =1;
		free(data);
		return a;
	}
	else if(!strcmp(data,"+++ killed by SIGIOT ")){
		strcpy(a->call,"killed by SIGIOT");
		a->end =1;
		free(data);
		return a;
	}
	strcpy(a->call,data);
	printf("On a un %s\n",a->call);
	sizec+=++i;
	i=0;
	
	/**Read unfinished or number of file descritor and the message*/
	int read_write = (!strncmp(a->call,"read",strlen("read")) || !strncmp(a->call,"write",strlen("write")));
	int begin_message = -1;
	while(i+1<size-sizec && (line[sizec+i]!='=' || line[sizec+i+1]!=' ')){
		if(line[sizec+i]=='<'){
			a->wait = 1;
			free(data);
			return a;
		}
		if(read_write){
			if(!a->fd){
				if(!a->fd && line[sizec+i]==','){
					data[i]='\0';
					a->fd = atoi(data);
				}
				else
					data[i]=line[sizec+i];
			}
			else if(begin_message!=-1){		
				if(begin_message && line[sizec+i]==','){
					data[i-begin_message]='\0';
					strcpy(a->message, data);
					read_write = 0;
				}			
				else
					data[i-begin_message] = line[sizec+i];
			}
			else if(line[sizec+i]=='"'){
				begin_message = i;
				data[i-begin_message] = line[sizec+i];
			}
		}
		i++;
	}
	
	sizec+=i+2;
	i=0;
	
	/**Read pid son*/
	while(i<size-sizec && line[sizec+i]!='\0' && line[sizec+i]!=' ' && line[sizec+i]!='\n'){
		data[i]=line[sizec+i];
		i++;
	}
	data[i]='\0';
	if(strcmp("?",data))
		a->pid_son = atoi(data);
	a->end =1;
	free(data);
	return a;
}

void writing_command(int fds,char* name_prog){
	if(!fork()){
  		creat("res/tmp.txt",0666);
  		int fdout = open("res/tmp.txt",O_WRONLY);
  		dup2(fdout,1);
    	execlp("strace","strace","-f","-e",TRACE_FORK,"-o","res/lala",name_prog, NULL);
  	}
  	else{
    	FILE* buffer = fopen("res/lala", "r");
    	char* buf=NULL;
    	char *line = malloc(MAX_LINE*sizeof(char));
    	action *action;
    	size_t len=0;
    	int got = 0;
    	creat("res/resultat.txt",0666);
    	creat("res/CLE1.txt",0666);
    	int fd = open("res/resultat.txt",O_WRONLY);
    	int fd1 = open("res/CLE1.txt",O_WRONLY);
    	while((got=getline(&buf, &len, buffer))!=-1){
			printf("Nouvelle ecriture niveau 1\n");
			write(fd,buf,got);
			action = parser_command(buf);
			sprintf(line,"%d/%d/%s/%d/%d/%d/%d/%s\n",action->begin,action->pid_father,action->call,action->wait,action->pid_son,action->end,action->fd,action->message);
			write(fd1,line,strlen(line));
			write(fds,line,strlen(line));
			free_action(action);
			if(global_data->run)
				usleep((MAX_SPEED-global_data->speed)*10000);
		}
		
    	printf("Fin niveau 1\n");
    	free(line);
    	free(buf);
    	close(fd);
  	}
  	//wait(NULL);
}

int readline (int fd, char *line, int maxlen){
	int n, rc, retvalue, still=1; 
	char c, *tmpptr=line; 
	for (n=1; (n < maxlen) && (still) ; n++) {
		if ( (rc = read (fd, &c, 1)) ==1) {
			*tmpptr++ =c; 
			if (c == '\n'){  
				still =0; 
				retvalue = n;
			}
		}
		else if (rc ==0) {  
			still = 0;
			if (n==1) 
				retvalue = 0;
			else 
				retvalue = n;	
		}
	}
	*tmpptr = '\0'; 
	return (retvalue);
}

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
	char *buf =malloc((MAX_LINE+1)*sizeof(char));
	char line[MAX_LINE];
	int *listing = malloc(50*sizeof(int));
	int *bool_listing = malloc(50*sizeof(int));
	int size_listing=0,num_turn=0,pos=-1;
	creat("res/CLE.txt",0666);
	int got=0,fd = open("res/CLE.txt", O_WRONLY),first_read=1;
	action *action;
	clear_listing(listing,50);
	clear_listing(bool_listing,50);
	do{
		printf("Boucle attente niveau 2\nrun : %d\tgot : %d\tstep_by_step : %d\n",global_data->run,got,global_data->step_by_step);
		if ((got = readline(fdl, buf, 500)) < 0){
      		got=1;
        }
		
		else if(got>0){
			printf("Message recu niveau 2\n");
			buf[got]='\0';
			/*Build action and manage the turns*/
			action = build_again_action(buf);
			if( ((first_read || !strlen(action->message)) && !strncmp(action->call,"read",strlen("read"))) || (!strlen(action->message) && !strncmp(action->call,"write",strlen("write"))) ){
				printf("On a un %s useless\n",action->call);
				first_read=0;
				free(action);
				continue;
			}
			else if((pos=isInListing(listing,size_listing,action->pid_father))==-1){
				listing[size_listing]=action->pid_father;
				size_listing++;
				bool_listing[size_listing-1]=1;
			}
			else{
				if(bool_listing[pos]){
					clear_listing(bool_listing,size_listing);
					bool_listing[pos]=1;
					num_turn++;
					char line2[50];
					sprintf(line2,"new turn\n");
					write(fdw,line2,strlen(line2));
					write(fd,line2,strlen(line2));
				}	
				else
					bool_listing[pos]=1;
			}
			sprintf(line,"%d/%d/%s/%d/%d/%d/%d/%s\n",action->begin,action->pid_father,action->call,action->wait,action->pid_son,action->end,action->fd,action->message);
			if((action->end && (strcmp(action->call,"clone") || action->pid_son!=0))|| !strcmp("wait4",action->call) || !strcmp("waitpid",action->call) || !strcmp("read",action->call) || !strcmp("write",action->call)){
				write(fdw,line,strlen(line));
			}
			write(fd,line,strlen(line));
			free_action(action);
		}
		if(global_data->run)
			usleep((MAX_SPEED-global_data->speed)*10000);
	}
	while(got>0);
	wait(NULL);
	printf("Fin lecture niveau 2\n");
	free(bool_listing);
	free(buf);
	close(fdw);
	free(listing);
}

void *execute_proc(void *pepp){
	/*Tester si le fifo existe et creer le fifo si necessaire*/
	mkdir("res",01777);
	mkfifo("res/lala",0666);
	param_execute_proc *pep = (param_execute_proc *) pepp;
	int fde[2];
	pipe(fde);
	if(!fork()){
		close(fde[0]);
		writing_command(fde[1],pep->binary);
		close(fde[1]);
	}
	else{
		close(fde[1]);
		read_info(fde[0],pep->fd);
		close(fde[0]);
	}
	
	return NULL;
}
