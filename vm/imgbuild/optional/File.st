Class File Object name number mode
Class FilePath Object base parent
Methods File 'all'
	asString	| text line |
		text <- ''.
		self mustNotBinary.
		[ (line <- self getString) notNil ]
			whileTrue: [ text <- text , line ].
		^ text
|
	dump |line|
		self mustNotBinary.
		[ (line <- self getString) notNil ]
			whileTrue: [ line print ]
|
	name: string
		name <- string
|
	name
		^ name
|
	getNumber
		" get a file number - called only by open"
		(1 to: 15) do: [:i | (files at: i) isNil
			ifTrue: [ files at: i put: self. number <- i. ^ nil]]
|
	open: m
		self mode: m.
		self open
|
	modes
		^ #('r' 'w' 'a' 'rb' 'wb' 'ab')
| 
	binMode
		^ (mode includes:$b)
|
	mustNotBinary
		self binMode ifTrue:[
			^self error:'File is in binary mode'].
|
	mode: m
		( self modes
			includes:m) ifFalse:[
			^self error:'Wrong mode : '
				, (self modes printString)
		].
		mode <- m
|
	open
		number notNil ifTrue: [ self close ].
		self getNumber.
		<120 number name mode> isNil
			ifTrue: [^ false].
		^ true
|
	close
		" close file, take entry out of global variable "
		number isNil ifTrue: [ ^ nil ].
		number < 3 ifTrue:[^nil].
		files at: number put: nil.
		<121 number>.
		number <- nil.
|
	getString
		self mustNotBinary.
		^ (number notNil)
			ifTrue: [<125 number>]
|
	read:length
		^ (<123 number length>)
|
	write:anArray
		^ (<124 number anArray>)
|
	print: aString
		self mustNotBinary.
		(number notNil)
			ifTrue: [<129 number aString>]
			ifFalse: [smalltalk error: 'file not open']
|
	printNoReturn: aString
		self mustNotBinary.
		(number notNil)
			ifTrue: [<128 number aString>]
			ifFalse: [smalltalk error: 'file not open']
|
	readUntil: conditionBlock doing: actionBlock	| line |
		self mustNotBinary.
		[ line <- self getString. line notNil]
			whileTrue: [ (conditionBlock value: line)
					ifTrue: [ ^ line ].
					actionBlock value: line ].
		^ nil
|
	do:aBlock
		aBlock value:self.
		self close
|
stdin
	name := 'stdin'.
	number := 1.
	mode := 'r'.
	^self
|
stdout
	name := 'stdout'.
	number := 2.
	mode := 'w'.
	^self
|
new
	mode <- 'r'
]
Methods FilePath 'all'
	new
		base <- '/'.
		parent <- nil
|
	basename
		^ base
|
	asPath
		^self
|
	basename:s
		base <- s
|
	parent:p
		parent <- p
|
	isRoot
		^ parent isNil and:[base = '/']
|
	parent
		^ parent
|
	dump
		self isDir ifTrue:[
			self ls] ifFalse:[
				(self asFile:'r') dump]
|
	 path
	 	parent isNil ifTrue:[^base].
		^ parent path , '/', base
|
	isFile
		^ self isDir not
|
	isDir
		self isRoot ifTrue:[^true].
		^ <126 1 (self path)>
|
	exist |f|
		self isRoot ifTrue:[^true].
		^ <126 0 (self path)>
|
	children |lk nx|
		self isFile ifTrue:[^Array new:0].
	 lk <- (<126 2 (self path)>).
	 nx <- lk.
	 [nx notNil] whileTrue:[
	 	nx value: (self/ (nx value)).
		nx <- nx next
	 ].
	 ^ (List new) links:lk
|
	ls
		self children do:[:e| e path print]
|
	/ aString
		aString isNil ifTrue:[^self].
		^(FilePath new) parent:self; 
			basename:aString
|
	asFile:m
		self isDir ifTrue:[
			^self error:'This path is a dir.'].
		^self path asFile:m
|
	mkdir:dName |p|
		self exist ifFalse:[
			self parent mkdir:self basename].
		dName isNil ifTrue:[^self].
		p <- self/dName.
		<126 3 (p path)> ifFalse:[
			^self error:'Cant not create directory:',dName].
		^ p
|
	mkdir
		^self mkdir:nil
|
	cpDirTo:anObj |p|
		p <- anObj asPath.
		p exist ifTrue:[
			p isFile ifTrue:[
				^self error:'Error cpy: DIR->file'].
			p <- p mkdir:self basename] ifFalse:[
			p <- p mkdir].
		self children do:[:e| 
			e cpTo:(p/(e basename))
		].
		^p
|
	cpTo:anObj |p|
		p <- anObj asPath.
		self isDir ifTrue:[
			^self cpDirTo:p] ifFalse:[
			p isDir ifTrue:[
				p <- p/(self basename)].
			<126 5 (self path) (p path)> ifFalse:[^self error:'Cannot copy file'].
			^ p
		]
|
	mvTo:anObj |p|
		p <- anObj asPath.
		self exist ifFalse:[
			^ self error:'File to move did not exist'].
		p isDir ifTrue:[
			p <- p/(self basename)].
		<126 6 (self path) (p path)> ifFalse:[
			^self error:'Cannot move the file'
		].
		^anObj
	
|
	mount
		<126 7> ifFalse:[
			self error:'Cannot mount root volume'].
|
	unmount
		<126 8> ifFalse:[
			self error:'Cannot unmount root volume'].
|
	delete
		self exist ifFalse:[^self parent].
		self isRoot ifTrue:[^self].
		self isDir ifTrue:[
			self children do:[:e| e delete]].
		<126 4 (self path)> ifFalse:[
			^self error:'Cant delete file'].
		^ self parent
]
Methods String 'all'
	asFile:m |f|
		f <- File new.
		f name:self.
		f open:m.
		^ f
|
	splitBy:pattern to:l |tmp idx|
		idx <- self indexOf:[:e| e = pattern].
		idx isNil ifTrue:[l add:self. ^self].
		idx = 1 ifFalse:[
			l add:(self copyFrom:1 to:idx-1)].
		idx = self size ifTrue:[^self].
		(self copyFrom:idx+1 to:self size) 
			splitBy:pattern to:l
|
	split:pattern |tmp idx l|
		l <- List new.
		self splitBy:pattern to:l.
		^l
|
	asPath |p l|
		p <- FilePath new.
		l <- self split:$/.
		l do:[:e|
			p <- p/e].
		^p
]