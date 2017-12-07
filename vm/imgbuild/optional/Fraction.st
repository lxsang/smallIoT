Class       Fraction Number top bottom
Methods Fraction 'all'
	= f
		f isFraction
			ifTrue: [ ^ (top = f top) and: [ bottom = f bottom ] ]
			ifFalse: [ ^ super = f ]
|
	< f
		f isFraction
			ifTrue: [ ^ (top * f bottom) < (bottom * f top) ]
			ifFalse:[ ^ super < f ]
|
	+ f
		f isFraction
			ifTrue: [ ^ ((top * f bottom) + (bottom * f top)) /
					(bottom * f bottom) ]
			ifFalse:[ ^ super + f ]
|
	- f
		f isFraction
			ifTrue: [ ^ ((top * f bottom) - (bottom * f top)) /
					(bottom * f bottom) ]
			ifFalse:[ ^ super - f ]
|
	* f
		f isFraction
			ifTrue: [ ^ (top * f top) / (bottom * f bottom) ]
			ifFalse: [ ^ super * f ]
|
	/ f
		^ self * f reciprocal
|
	abs
		^ top abs / bottom
|
	asFloat
		" convert to a floating point number "

		^ top asFloat / bottom asFloat
|
	truncated
		" convert to an integer rounded towards zero "
		^ top quo: bottom
|
	bottom
		^ bottom
|
	coerce: x
		" coerce a value into being a fraction "

		^ x asFraction
|
	generality
		" generality value - used in mixed type arithmetic "
		^ 5
|
	isFraction
		^ true
|
	ln
		^ (top ln) - (bottom ln)
|
	raisedTo: x
		^ (top raisedTo: x) / (bottom raisedTo: x)
|
	reciprocal
		^ bottom / top
|
	top
		^ top
|
	with: t over: b
		" initialization "

		top <- t.
		bottom <- b
|
	printString
		^ top printString, '/', bottom printString
]
Methods Integer 'all' 
	asFraction
		^ Fraction new ; with: self over: 1
|
	/ value		| t b |
		value = 0 ifTrue: [ ^ smalltalk error: 'division by zero'].

		value isInteger
			ifTrue: [ b <- self gcd: value .
				  t <- self quo: b.
				  b <- value quo: b.
				  b negative
					ifTrue: [ t <- t negated. 
						  b <- b negated ].
				  (b = 1) ifTrue: [ ^ t ].
				  ^ Fraction new; with: t over: b ]
			ifFalse: [ ^ super / value ]
]
Methods Number 'all'
	fractionalPart
		^ self - self truncated
]