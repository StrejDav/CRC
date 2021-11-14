

int gray(int n)
{
  n--;
  return n ^ (n >> 1);
}

int main()
{
    int c = gray(4);

    return 0;
}