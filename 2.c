int fact(int x){
	if(x==1)
		return x;
	else 
		return x*fact(x-1);
}

int main(){
	int m,result;
	m=read();
	if(m>1)
		result=fact(m);
	else
		result=1;
	write(result);
	return 0;
}
