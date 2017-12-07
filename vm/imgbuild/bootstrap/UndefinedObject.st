*Class UndefinedObject Object

Methods UndefinedObject 'all'
	isNil
		^ true
|
	notNil
		^ false
|
	printString
		^ 'nil'
|
	createGlobals	| aBlock str|
		" create global variables in initial image "
		true <- True new.
		false <- False new.
		smalltalk <- Smalltalk new.
		serial <- Serial new.
		root <- FilePath new.
		files <- Array new:15.
		files at:1 put:(File new stdin).
		files at:2 put:(File new stdout).
		stdin <- files at:1.
		stdout <- files at:2.
		" create a dictionary of classes "
		classes <- Dictionary new.
		scheduler <- Scheduler new.
		symbols binaryDo: [:x :y | 
			(y class == Class)
				ifTrue: [ classes at: x put: y ] ].
		aBlock <- [
			systemProcess <- aBlock newProcess.
			scheduler run ].
		systemProcess <- aBlock newProcess.
	
|
	initialize
		" initialize the initial object image "
		self createGlobals.
]