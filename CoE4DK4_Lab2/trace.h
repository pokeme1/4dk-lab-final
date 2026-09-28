#ifndef TRACE_H
#define TRACE_H

#ifdef ENABLE_TRACE
#define TRACE(x) do { x; } while (0);
#else
#define TRACE(x) do { } while (0);
#endif

#endif /* TRACE_H */
