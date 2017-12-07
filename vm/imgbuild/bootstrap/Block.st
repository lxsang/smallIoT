*Class Block Object context argCount argLoc bytePointer
Methods Block 'all'
	newProcess 
		" create a new process to execute block "
		^ Process new; context: context ; startAt: bytePointer.

|

timesToRun
	^(<153 2 self>)

|

	newProcessWith: args
		(self checkArgumentCount: args size)
			ifTrue: [ (1 to: args size) do: [:i |
				   context at: (argLoc + i - 1) 
					put: (args at: i)]].
		^ self newProcess

|

	value: x value: y
		^ (self checkArgumentCount: 2)
			ifTrue: [ context at: argLoc put: x.
				  context at: argLoc + 1 put: y.
				  context returnToBlock: bytePointer ]

|

	fork
		self newProcess resume

|

	whileTrue
		self whileTrue: []

|

	forkWith: args
		(self newProcessWith: args) resume

|
	checkArgumentCount: count
		^ (argCount = count)
			ifTrue: [ true ]
			ifFalse: [ smalltalk error:
				'wrong number of arguments passed to block'.
				false ]

|

	value: x
		^ (self checkArgumentCount:  1)
			ifTrue: [ context at: argLoc put: x.
				  context returnToBlock: bytePointer ]

|

	value: x value: y value: z
		^ (self checkArgumentCount:  3)
			ifTrue: [ context at: argLoc put: x.
				  context at: argLoc + 1 put: y.
				  context at: argLoc + 2 put: z.
				  context returnToBlock: bytePointer ]

|

	whileFalse: aBlock
		[ self value not ] whileTrue: aBlock

|

	blockContext: ctx
		context <- ctx

|

	whileTrue: aBlock
		( self value ) ifTrue:
			[ aBlock value. 
				self whileTrue: aBlock ]

|

	value
		^ (self checkArgumentCount: 0)
			ifTrue: [ context returnToBlock: bytePointer ]

]
