#ifndef TSYS_API_H
#define TSYS_API_H

#ifdef TSYS_API_EXPORT
#define TSYS_API __declspec(dllexport)
#else
#define TSYS_API __declspec(dllimport)
#endif

#endif //TSYS_API_H
