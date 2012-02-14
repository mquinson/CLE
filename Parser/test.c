/**
 * @lesson logo
 * @exercise square
* @world 200 200
* 
* @description lalalalalalalalalalallalalalalaallal la
* 
*on rajout un truc pour l'honneur
* @entity 100 100 0.
*/

  /*BEGIN TEMPLATE*/
  /*BEGIN SOLUTION*/
void square(int size) {
	int i;
	for (i=0;i<4;i++) {
		forward(size);
		right(90);
	}
}

void run (){
	square(90);
	square(60);
	square(30);
}

/*END SOLUTION*/
/*END TEMPLATE*/