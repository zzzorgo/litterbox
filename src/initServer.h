#ifndef LiterboxServer
#define LiterboxServer

void serverBegin();
void handleClient(bool (*pushData)(Print*));

#endif
