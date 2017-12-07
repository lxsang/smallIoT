Class Process Object stack stackTop linkPointer
Methods Process 'all'
	new
		stack <- Array new: 50.
		stackTop <- 10.
		linkPointer <- 2.
		stack at: 4 put: 1. "return point"
		stack at: 6 put: 1. "bytecode counter"

|

	context
		^ stack at: 3

|

	trace		| link m r s |
		" first yield scheduler, forceing store of linkPointer"
		scheduler yield.
		link <- linkPointer.
		link <- stack at: link+1.
		" then trace back chain "
		[ link notNil ] whileTrue:
			[ 
			m <- stack at: link+3.
			  m notNil 
				ifTrue: [ 
					s <- (m signature, ' (').
			  		  r <- stack at: link+2. 
			  		  (r to: link-1) do: 
						[:x | s <- s, ' ', 
							(stack at: x) class asString].
					  (s, ')') print ].
			  link <- stack at: link ]

|

	method: x 
		stack at: 5 put: x.

|

	startAt: x
		stack at: 6 put: x. "starting bytecode value"

|
	execute 
		" execute for time slice, terminating if all over "
		(stack size > 1500)
			ifTrue: [ smalltalk error:
				'process stack overflow, probable loop'].
		<19 self> ifTrue: [] ifFalse: [ self terminate ].

|

	terminate
		" kill current process "
		scheduler removeProcess: self. scheduler yield.

|

	resume
		" resume current process "
		scheduler addProcess: self

|

	context: ctx
		stack at: 3 put: ctx.

]
