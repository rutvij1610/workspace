/*
Prologue:
The code takes an edge weighted,connected graph from a user-specified input file and gives a Minimum weight Spanning Tree of the same in a user-specified output file. The code makes use of greedy algorithm i.e. at every step the edge with minimum weight is chosen out of the set of edges which have not yet been considered for inclusion in the Minimum weight Spanning Tree. The minimum-weight edge chosen is included in the MST(Minimum Spanning Tree) if its inclusion does not result in the formation of a cycle. In this way n-1 edges are chosen where n is the number of vertices.

Program Overview:
The program consists of the following structures and functions:
(1.) An edge is represented by the structure:
struct edge
{
  int i;
  int j;
  double k;
} 
'i' and 'j' denote the vertices connected by the edge. 'k' is the edge-weight.

(2.) int cmpr;
'cmpr' is a count of the number of comparisons.

(3.)struct edge min_wt_edge(struct edge* e, int l);
It determines the edge with minimum weight from an array of edges using heap-sort.

(4.)void min_heapify(struct edge* e, int l);
It converts 'e', an array of edges, into a min-heap. 'l' is the index of the last element in the array which (edge) will be a part of the resulting min-heap.

(5.)void siftdown(struct edge* e,int i,int l);
It is called by min-heapify function. It sifts down the ith element in the array. 

(6.)int is_acyclic(struct edge min_e, int* arr);
It determines whether inclusion of the edge 'min_e' in the set of edges for MST results in a cycle (graph). It returns 'false' if it does and 'true' when it does not. 'arr', an integer array of size 'n' is used for the implementation of the SET:UNION NAD FIND algorithm, where 'n' is the number of vertices. 
Initially the kth element of 'arr' has value k. 
If vertices p,q,r,s are connected then:
arr[p]=arr[q]=arr[r]=arr[s]=min(p,q,r,s) 
where 1 <= p,q,r,s <= n.         
*/

#include<stdio.h>
struct edge
{
   int i;
   int j;  
   /*i and j are vertices*/
   double k; /*edge weight*/
} ;

int cmpr=0;

struct edge min_wt_edge(struct edge* e, int l);
void min_heapify(struct edge* e, int l);
void siftdown(struct edge* e,int i,int l);
int is_acyclic(struct edge min_e, int*);
void main()
{
   
   /*reading from input file*/
   FILE* f;
   char fname[30];
   printf("Enter the input filename: ");
   scanf("%s",fname);
   f=fopen(fname, "r");
   int n; /*number of vertices*/
   fscanf(f,"%d",&n);
   char a,b,c,d,z;
   fscanf(f,"%c",&a);
   int m; /*number of edges*/
   fscanf(f,"%d",&m);
   struct edge e[m];
   int g;
   a=fgetc(f);
   for(g=0;g<m;++g)
    {
       fscanf(f,"%c""%d""%c""%d""%c""%lE""%c""%c",&a,&e[g].i,&b,&e[g].j,&c,&e[g].k,&d,&z);
        
    }
   fclose(f);
    
   int arr[n];
   int p=0;
   /*initializing arr*/
   for(p=0;p<n;++p)
    {
       arr[p]=p+1;  
    } 
   int h =0;
   int i=0;
   int j;
   int count=0; 
   struct edge mst[n-1];
   struct edge min_e;
   while(h<(n-1))
     {
         /*select the minimum weight edge*/
         min_e=min_wt_edge(e,m-count);
         ++count;
         
         /*check if minimum weight edge is acyclic*/
         j=is_acyclic(min_e,arr); 
         if(j==1)
          {
             /*changes in arr*/
             ++cmpr; 
             if(arr[min_e.i-1]<arr[min_e.j-1])
              for(i=0;i<n;++i)
               {
                   if(arr[i]==arr[min_e.j-1])
                   arr[i]=arr[min_e.i-1];
               }
             else
              for(i=0;i<n;++i)
               {
                   if(arr[i]==arr[min_e.i-1])
                   arr[i]=arr[min_e.j-1];
               }


             /*changes in mst*/
             mst[h]=min_e;
             ++h;
          } 
     }
  
   /*Printing output in the output file*/
   int y;
   double weight=0;
   printf("Enter the output file name: ");
   scanf("%s",fname);
   printf("\n"); 
   f=fopen(fname,"w");
   /*printing the input graph*/
   fprintf(f,"Input Graph:\n");
   fprintf(f,"%d,%d\n",n,m);
   for(y=0;y<m;++y)
    {
       fprintf(f,"(%d,%d,%lE)\n",e[y].i,e[y].j,e[y].k); 
    } 
   fprintf(f,"\nOutput Graph:\n");
   fprintf(f,"%d,%d\n",n,n-1);
   for(y=0;y<(n-1);++y)
    {
       fprintf(f,"(%d,%d,%lE)\n",e[y].i,e[y].j,e[y].k);
       weight+=e[y].k; 
    } 
   
   fprintf(f,"The weight of the Minimum weight Spanning Tree: %lE\n",weight);
   fprintf(f,"Number of comparisons: %d",cmpr);
   
}

struct edge min_wt_edge(struct edge* e,int l)
{
     struct edge temp;
     min_heapify(e,l);
     temp=e[0];
     e[0]=e[l-1];
     e[l-1]=temp;
     return e[l-1];
}

void min_heapify(struct edge* e,int l)
{
     int i;
     for(i=(l/2);i>=1;--i)
      {
         siftdown(e,i,l);  
      }
}

void siftdown(struct edge* e,int i,int l)
{
      int min;
      struct edge temp;
      while(1)
       {
          if(2*i>l)
           break;
          else
           { 
              if((2*i)==l)
               {
                  ++cmpr;
                  if(e[(2*i)-1].k<e[i-1].k)
                   {
                      /*swapping of values*/ 
                      temp=e[i-1];
                      e[i-1]=e[(2*i)-1];
                      e[(2*i)-1]=temp;       
                   }
               }
              else
               {
                  ++cmpr;
                  if(e[(2*i)-1].k<e[(2*i)].k)
                   min=2*i;
                  else
                   min=(2*i)+1;

                  ++cmpr; 
                  if(e[i-1].k>e[min-1].k)
                   {
                      temp=e[i-1];
                      e[i-1]=e[min-1];
                      e[min-1]=temp;
                      
                      i=min;
                      continue;  
                   }   
               }
             
              break;  
           }
       }
}

int is_acyclic(struct edge min_e, int* arr)
{
    ++cmpr;
    if(arr[min_e.i-1]!=arr[min_e.j-1])
     return 1;
    else 
     return 0;  
}
