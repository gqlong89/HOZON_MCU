
unsigned int shellRedirectBufSize;
char *shellRedirectBuf;
unsigned int redirectLen;

void setshellRedirectBufSize(unsigned int size)
{
    shellRedirectBufSize = size;
}

unsigned int getshellRedirectBufSize(void)
{
    return shellRedirectBufSize;
}

void setshellRedirectBuf(char *buf)
{
    shellRedirectBuf = buf;
}

void stopshellRedirect(void)
{
    shellRedirectBufSize = 0;
    shellRedirectBuf = 0;
    redirectLen = 0;
}
