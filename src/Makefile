PROG=	dsvn

SRCS=	dsvn.c
OBJS=	${SRCS:.c=.o}

all:	${PROG}

${PROG}:	${OBJS}
	${CC} -ldialog ${OBJS} -o ${PROG}

.for _obj in ${OBJS}
${_obj}:	${_obj:.o=.c}
	${CC} -c -g -Wall -Werror -o ${.TARGET} ${.TARGET:.o=.c}
.endfor

clean:
	@rm -f ${OBJS} ${PROG}
