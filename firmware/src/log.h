#ifndef LOG_H
#define LOG_H

/* variables */

/* functions declaration */

/*******************************************************************************
 * Function Name  : log_to_evaluator
 * Description    : Function used to log data to the evaluator computer
 * Input          : Variadic function, same arguments as you would give to
 *                  printf()
 * Return         : None
 *******************************************************************************/
void log_to_evaluator(const char *fmt, ...);

#endif /* LOG_H*/
