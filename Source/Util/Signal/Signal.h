#ifndef SIGNAL_H
#define SIGNAL_H

// NOTE:
// RECORDAR QUE TODOS LOS DATOS QUE SE MANDAN SON PUNTEROS!!
// SI AL PROCESARLOS SE NECESITAN GUARDAR ES NECESARIO HACERSE UNA COPIA
// YA QUE EN EL SIGUIENTE FRAME PUEDE QUE ESE PUNTERO YA NO VALGA
// 
// AUN ASI LO L�GICO ES PROCESAR LA SE�AL EN EL MOMENTO QUE SE RECIBE, NO LUEGO

enum TSignal {
	SIGNAL_NONE = 0

	// Se�ales que manda el player a toda su lista de listeners
	, SIGNAL_PLAYER_DO_KICK  // Sin datos

	// Se�ales que le mandan al player y el player las procesa
	, SIGNAL_HIT_PLAYER      // Datos: Direccion en que golpearon al player
};

class SignalListener
{
public:
	SignalListener( ) {}
	virtual ~SignalListener( ) {}

	void processSignal( TSignal tag ) { processSignal( tag, 0, 0 ); }
	virtual void processSignal( TSignal tag, const void *data, size_t num_bytes ) {}
};

#endif // SIGNAL_H
