*Class Smalltalk Object
Methods Smalltalk 'all'
	perform: message withArguments: args ifError: aBlock	
			| receiver method |
		receiver <- args at: 1 ifAbsent: [ ^ aBlock value ].
		method <- receiver class methodNamed: message.
		^ method notNil 
			ifTrue: [ method executeWith: args ]
			ifFalse: aBlock
|
	perform: message withArguments: args
		^ self perform: message withArguments: args
			ifError: [ self error: 'cant perform' ]
|
	watch
		^ <5>
|
	getPrompt: aString
		stdout printNoReturn: aString.
		^ stdin getString
|
	inquire: aString	| response |
		response <- self getPrompt: aString.
		response isNil
			ifTrue: [ ^ false ].
		^ 'Yy' includes: (response at: 1 ifAbsent: [])
|
	echo
		" enable - disable echo input "
		echoInput <- echoInput not
|
	error: aString
		" print a message, and remove current process "
			stdout print: aString.
			scheduler currentProcess isNil ifFalse:[
				scheduler currentProcess trace; terminate
			].
		^ aString
|
	ws |str|
		str <- (<122 '' 0>).
		str notNil ifTrue:[
			str doIt
		]
]