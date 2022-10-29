#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define null NULL


//list elements
typedef struct celulag
{
  void* info;           /* info address */
  struct celulag *urm;   /* addres of next cell */
} TCelulaG, *TLG; 

typedef struct {

	TLG 	b;			
	TLG 	vf; 		

} TStiva, *ASt;

typedef struct{

	TLG 	ic; 		
	TLG 	sc; 	

} TCoadaV, *AQV;

//queue elements
typedef struct{
	
	int id; //the id
	int timp; //necessary time of execution
	int pr; //priority
	int rth; //thread on which it was executed
	int tpr; //time left for execution

}ElCoada;

//stack elements
typedef struct{

	int id; //task id 
	int timp; //necessary time for execution
	int tpr; //remaining time for execution
	int indice; //thread number
	int pr; //task priority

}ElStiva;


TLG GenCLS(int x){ //stack cell generator

	TLG aux;
  	aux = (TLG)malloc(sizeof(TCelulaG));
   	if(!aux) return NULL;
   	aux->info = calloc(1, sizeof(ElStiva));
	ElStiva *e = (ElStiva *) aux->info;
	e->indice = x;
	e->timp = 0;
	e->id = 0;
	e->pr = 0;
	e->tpr = 0;
	aux->urm = NULL;
	return aux;
}

TLG GenCLC(int tp, int pr, int id){ //queue cell generator

	TLG aux = malloc(sizeof(TCelulaG));
	if(!aux) return 0;
	aux->urm = NULL;
	aux->info = calloc(1, sizeof(ElCoada));
	ElCoada *c = (ElCoada *) aux->info;
	c->timp = tp;
	c->id = id;
	c->pr = pr;
	c->rth = -1;
	c->tpr = tp;
	return aux;
}

int PushS(ASt *s, TLG a){ //adds to the stack

	if((*s)->vf == null){
		a->urm = null;
		(*s)->b = a;
	}
	a->urm = (*s)->vf;
	(*s)->vf = a;
	return 1;
}

TLG PopS(ASt *s){ //pops the cell from stack

	TLG aux = NULL;
	aux = (*s)->vf;
	(*s)->vf = (*s)->vf->urm;
	aux->urm = null;
	return aux;

}

TLG ExtrC(AQV *c){ //pops the cell from queue

    TLG aux = null;
    if((*c)->ic == null)
        return null;
	aux = (*c)->ic;
	(*c)->ic = (*c)->ic->urm;
    aux->urm = null;
	return aux; 

}

int AdaugaC(AQV *c, TLG a){ //adds sorted in queue
	
    AQV aux = calloc(1,sizeof(TCoadaV));
    ElCoada *ea = (ElCoada *) a->info;
    if((*c)->ic == null){
    
        (*c)->ic = a;
        (*c)->sc = a;
        return 1;
    }
    int ok = 0, kk = 0;

    TLG p = null;
    while((*c)->ic != null){
        ok = 0;
	
        p = ExtrC(c);
		p->urm = null;
        ElCoada *ec = (ElCoada *)p->info;
		//we use an auxiliary queue for sorting 
		if(ea->pr > ec->pr)
            ok = 1;
        else
            if(ea->pr == ec->pr && ea->timp < ec->timp)
                ok = 1;
            else
                if(ea->timp == ec->timp && ea->id < ec->id)
                    ok = 1;
        if(ok == 1 && kk == 0){
            kk = 1;
            if(aux->ic == null){
                aux->sc = a;
                aux->ic = a;
            }
            else{
                aux->sc->urm = a;
                aux->sc = a;
            }
        }
        if(aux->ic == null){
            aux->ic = p;
            aux->sc = p;
        }
        else{
            aux->sc->urm = p;
        	aux->sc = p;
        }
    }

    if(ok == 0){
        aux->sc->urm = a;
        aux->sc = a;
    }

    (*c)->ic = aux->ic;
    (*c)->sc = aux->sc;

    free(aux);
	return 1;


}

void printn(AQV c){
	//useful for debugging 
	TLG a = null;
	 a = c->ic;
    for(; a != null; a = a->urm){
        ElCoada *e = (ElCoada *)a->info;
        printf("%d %d %d\n", e->id, e->timp, e->pr);
    }
}

int add_tasks(AQV *c, char *content, int8_t *fr, int nrfr, FILE *fo){ 
	//first function for insertion

	char *p = strtok(content," ");
	int nr_taskuri = 0;
	int i;
	for(i = 0; p[i] != 0; i++)
		nr_taskuri = nr_taskuri * 10 + p[i] - '0';

	p = strtok(0," ");
	int timp = 0;
	for( i = 0; p[i] != 0; i++)
		timp = timp * 10 + p[i] - '0';

	p = strtok(0," \n");
	int priority = 0;

	for( i = 0; p[i] != 0; i++)
		priority = priority * 10 + p[i] - '0';

	for(i = 1; i <= nrfr; i++){
		if(*(fr + i) == 0 && nr_taskuri != 0){
			nr_taskuri--;
			TLG a = GenCLC(timp, priority, i);
			ElCoada *e = (ElCoada *)a->info;
			AdaugaC(c, a);
			*(fr + i) = 1; 	
			fprintf(fo, "Task created successfully : ID %d.\n", i);
		}
	}	
	if(nr_taskuri != 0){
		int i;
		nrfr += nr_taskuri;
		for(i = 1; i <= nrfr; i++){
			if(*(fr + i) == 0 && nr_taskuri != 0){
				nr_taskuri--;
				TLG a = GenCLC(timp, priority, i);
				ElCoada *e = (ElCoada *)a->info;
				AdaugaC(c, a);
				*(fr + i) = 1;
				fprintf(fo, "Task created successfully : ID %d.\n", i);
				}
			}	
	}
	return nrfr;//returns the number of tasks from queue
}

void PrintW(AQV *c, FILE *fo){

	//prints waiting list
	fprintf(fo, "====== Waiting queue =======\n[");
	AQV ax = calloc(1,sizeof(TCoadaV));
	while((*c)->ic != NULL){
		TLG aux = ExtrC(c);
		ElCoada *e = (ElCoada *)aux->info;
		fprintf(fo, "(%d: priority = %d, ", e->id, e->pr);
		fprintf(fo, "remaining_time = %d)", e->timp);
		if((*c)->ic != null)
			fprintf(fo, ",\n");
		if(ax->ic == null){
			ax->ic = aux;
			ax->sc = aux;
		}
		else{
			ax->sc->urm = aux;
			ax->sc = aux;
		}
	}

	fprintf(fo, "]\n");
	*c = ax;
}

int get_task(AQV *c, int id, int ind, FILE *fo){

	if((*c)->ic == null){
		return 0;
	}
	int ok = 0;
	AQV ax = calloc(1, sizeof(TCoadaV));
	while((*c)->ic != null){
		TLG p = ExtrC(c);
		ElCoada *e = (ElCoada *)p->info;
		if(e->id == id){
			switch(ind){
				case 1:
					fprintf(fo, "Task %d is waiting ", e->id);
					fprintf(fo, "(remaining_time = %d).\n", e->timp);
				break;
				case 2:
					fprintf(fo, "Task %d is running ", e->id);
					fprintf(fo, "(remaining_time = %d).\n", e->timp);
				break;
				case 3:
					fprintf(fo, "Task %d is finished ", e->id);
					fprintf(fo, "(executed_time = %d).\n", e->timp);
				break;
			}
			ok = 1;
		}
		if(ax->ic == null){
			ax->ic = p;
			ax->sc = p;
		}
		else{
			ax->sc->urm = p;
			ax->sc = p;
		}
	}
	*c = ax;
	return ok;
}

void prints(ASt *s, FILE *fo){

	//useless function for the program
	ASt aux = calloc(1, sizeof(TStiva));
	while((*s)->vf != null){
		TLG p = PopS(s);
		ElStiva *e = (ElStiva *) p->info;
		fprintf(fo, "indice %d\n", e->indice);
		PushS(&aux, p);
	}
	while(aux->vf != null){
		TLG p = PopS(&aux);
		PushS(s, p);
	}
}

int AddC(AQV *c, TLG a){
	//adds a cell in queue
	if((*c)->ic == null){
		(*c)->ic = a;
		(*c)->sc = a;
		(*c)->sc->urm = null;
	}
	else{
		(*c)->sc->urm = a;
		(*c)->sc = a;
	}
	return 1;

}

void get_thread(ASt *s, int ind, FILE *fo, int n){
	//prints thread
	if((*s)->vf == null){
		printf("nuj cum dar e goala\n");
		return;
	}
	ASt aux = calloc(1, sizeof(TStiva));
	int i;
	for( i = 0; i < n; i++){
		TLG p = PopS(s);
		if(i == ind){
			ElStiva *e = (ElStiva *) p->info;
			if(e->id == 0 || e->tpr <= 0)
				fprintf(fo, "Thread %d is idle.\n", ind);
			else
			{
				fprintf(fo, 
				"Thread %d is running task %d (remaining_time = %d).\n", 
				ind, e->id, e->tpr);
			}
		}
		PushS(&aux, p);
	}
	while(aux->vf != null){
		TLG p = PopS(&aux);
		PushS(s, p);
	}
}

int run(AQV *c1, AQV *c2, AQV *c3, ASt *s, int tp, FILE *fo, int n, int8_t *fr)
{
	int i;
	/*when we run, we start by checking if the stack has available places;
	then we add new tasks, we substract the time and see if any of the tasks are done 
	and move them to finished list*/
	ASt saux = calloc(1, sizeof(TStiva));
	for( i = 0; i < n ; i++){
        TLG ps = PopS(s);

		ElStiva *es = (ElStiva *)ps->info;
		if(es->tpr <= 0){
			TLG pc1 = ExtrC(c1);
			if(pc1 == null)
				break;
			AddC(c2, pc1);
			ElCoada *ec1 = (ElCoada *)pc1->info;
			*(fr + ec1->id) = 0;
			es->id = ec1->id;
			es->timp = ec1->timp;
			es->tpr = ec1->timp;
			es->pr = ec1->pr;
			ec1->rth = i;
			ec1->tpr -= tp;
		}
		es->tpr -= tp;
        int ok = 0;
		if(es->tpr <= 0){
			if(es->id != 0){
				AQV craux = calloc(1, sizeof(TCoadaV));
				TLG pc = ExtrC(c2);
				
				while((*c2)->ic != null){
					AddC(&craux, pc);
					pc = ExtrC(c2);
				}

				*c2 = craux;
				AddC(c3, pc);
				
			}
		}
		PushS(&saux, ps);
	}
	fprintf(fo,"Running tasks for %d ms...\n", tp);
	while(saux->vf != null){
		TLG p = PopS(&saux);
		PushS(s, p);
	}
	return 1;
}

void PrintR(AQV *c, FILE *fo){

	//prints running list
	fprintf(fo, "====== Running in parallel =======\n[");
	if((*c)->ic == null){
		fprintf(fo, "]\n");
		return;
	}
	AQV craux = calloc(1, sizeof(TCoadaV));
	while((*c)->ic != null){
		TLG p = ExtrC(c);
		ElCoada *e = (ElCoada *)p->info;
		fprintf(fo, "(%d: priority = %d, remaining_time", e->id, e->pr);
		fprintf(fo, " = %d, running_thread = %d)", e->tpr, e->rth);
		if(p->urm != null)
			fprintf(fo, ",\n");
		AddC(&craux, p);
	}
	fprintf(fo, "]\n");
	*c = craux;
}

void PrintF(AQV *c, FILE *fo){

	//prints finished list
	fprintf(fo, "====== Finished queue =======\n[");
	if((*c)->ic == null){
		fprintf(fo, "]\n");
		return;
	}
	AQV craux = calloc(1, sizeof(TCoadaV));
	while((*c)->ic != null){
		TLG p = ExtrC(c);
		ElCoada *e = (ElCoada *)p->info;
		fprintf(fo, "(%d: priority = %d, executed_time = %d)", e->id, e->pr, 
		e->timp);
		if(p->urm != null)
			fprintf(fo, ",\n");
		AddC(&craux, p);
	}
	fprintf(fo, "]\n");
	*c = craux;

}

int finished(AQV c){

	//this should find how much time is necessary for all tasks to finish
	if(c->ic == null)
		return 0;
	int m = 0;
	while(c->ic != null){
		TLG p = ExtrC(&c);
		ElCoada *e = (ElCoada *) p->info;
		if(m < e->timp)
			m = e->timp;
	}
	return m;

}

int main(int narg, char *nume[]){

	FILE *f = fopen(nume[1], "rt");
	if(!f){
		printf("nu vrea sa se deschida\n");
		return 0;
	}

	FILE *fo = fopen(nume[2], "wt");
	if(!fo){
		printf("nu vrea sa deschida output-ul\n");
		return 0;
	}
	int q, n, c;
	char *line = NULL;
	size_t len = 0;
	fscanf(f,"%d", &q);
	fscanf(f,"%d", &c);
	n = 2 * c;
	int8_t *fr = calloc(32678, sizeof(int8_t));
	*fr = 1;
	int nrfr = 0;
	ASt s = calloc(1, sizeof(TStiva));
	int i;

	for( i = n-1; i >= 0; i--){
		TLG aux = GenCLS(i);
		PushS(&s, aux);			
	}
	int tpt = 0;

	AQV c1,c2,c3;
	c1 = malloc(sizeof(TCoadaV));
	c2 = malloc(sizeof(TCoadaV));
	c3 = malloc(sizeof(TCoadaV));
	int sf = 0;
	while(getline(&line, &len, f) != -1){
		char *command = strtok(line," ");
		char *content = strtok(0,"\n");

		if(content != null)
		if(command[0] == 'a'){
			nrfr = add_tasks(&c1, content, fr, nrfr, fo);
		}
		if(command[0] == 'p'){

			if(strcmp(content, "waiting") == 0)
				PrintW(&c1, fo);
			if(strcmp(content, "running") == 0)
				PrintR(&c2, fo);
			if(strcmp(content, "finished") == 0)
				PrintF(&c3, fo);
				
		}
		if(strcmp(command,"get_task") == 0){
			int id = 0;
			for( i = 0; content[i] != 0; i++)
				id = id * 10 + content[i] - '0';
			int ok = 0;
			ok += get_task(&c1, id, 1, fo);
			ok += get_task(&c2, id, 2, fo);
			ok += get_task(&c3, id, 3, fo);
			if(ok == 0)
				fprintf(fo ,"Task %d not found.\n", id);			
		}
		if(strcmp(command, "get_thread") == 0){
			int id = 0;
			for( i = 0; content[i] != 0 && content[i] <= '9' && 
			content[i] >= '0'; i++){
				id = id * 10 + content[i] - '0';
			}
			get_thread(&s, id, fo, n);			
		}
		if(strcmp(command, "run") == 0){
			int tp = 0;
			for( i = 0; content[i] != 0; i++)
				tp = tp * 10 + content[i] - '0';
			run(&c1, &c2, &c3, &s, tp, fo, n, fr);
			sf += tp;
		}
		if(strcmp(command, "finish") == 0){
			sf += finished(c1);
			fprintf(fo, "Total time: %d\n", sf);
			break;
		}

	}	
	fclose(f);
	fclose(fo);

    return 0;
}