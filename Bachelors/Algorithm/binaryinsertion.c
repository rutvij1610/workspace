#include <stdio.h>
binaryinsert (int A[], int i)
{
  int j = 0, k = 0,temp=0;
  if (A[i - 1] > A[i])
    { k = mbsearch (A, i);
      for (j = i; j > k; j--)
	{ temp = A[j];
	  A[j] = A[j-1];
	  A[j-1] = temp;
	}
    }
}
mbsearch (int A[], int i)
{ int left = 0;
  int right = i;
  int mid = 0;
  while (left < right)
    {
      mid = (left + right) / 2;
      if (A[i] >= A[mid])
	left = mid + 1;
      else
	right = mid;
    }
printf("%dvalkue of key is",left);
  return left;
}


main ()
{
  FILE *fin;
  char filename[20];
  int ch,num,length,pl;
  int i = 0, j = 0, k, size=0, q = 0;	//
  printf ("enter name of input file\n");
  scanf ("%s", filename);
  printf ("input file is %s\n", filename);

//opening input file
  fin = fopen (filename, "r");
  if (fin == NULL)
    {
      printf ("error");

    }
  rewind (fin);

//determing size of A1
  while (1)
    {
      fscanf (fin, "%d \n", &ch);
      if (!feof(fin))
	i++;
      else
	break;
    }
num=i;length=i;
//making aaarry
  int A[i];

  for (q = 0; q < num; q++)
    {
      A[q] = 0;
    }
//storing values in array
  i = 0;
  rewind (fin);
  printf ("----------------------------------------\n");
  while (1)
    {
      fscanf (fin, "%d \n", &ch);
      if (!feof(fin))
	{
	  A[i] = ch;
	  printf ("stored array- %d\t%d\n", i,A[i]);
	  i++;
	}
	
	else
	break;
    }
A[i]=ch;
printf("%d\n",A[7]);  
fclose (fin);
  printf ("---------file read and stored-------------------------------\n");
//binaryinsertsort(A,n)
  for (j = 1; j <= num; j++)
    binaryinsert (A, j);
//writing output in file
  FILE *out;
  out = fopen ("output.txt", "w+");
  printf ("----------------------------------------\n");
  printf ("sorted array is:\n");
//printing output
printf("size of array is %d\n",++length);
  for (k = 0; k <=num; k++)//
    {
      printf ("%d\n", A[k]);
      fprintf (out, "%d\n", A[k]);
    }

  printf ("----------------------------------------\n");
}
