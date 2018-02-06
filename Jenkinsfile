node {
	stage 'Checkout'
		checkout scm

	stage 'Build'
		RUN 'cd linux'
		RUN "sed -i 's+/home/blue/code/EtherTerm/src/+../src/+' EtherTerm.mk"
    		RUN "sed -i 's+/home/merc/code/EtherTerm/src/+../src/+' EtherTerm.mk"
    		RUN 'make clean'
    		RUN 'make -j3'
}
