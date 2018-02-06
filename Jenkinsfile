node {
	stage 'Checkout'
		checkout scm

	stage 'Build'
		sh 'cd linux'
		sh "sed -i 's+/home/blue/code/EtherTerm/src/+../src/+' EtherTerm.mk"
    sh "sed -i 's+/home/merc/code/EtherTerm/src/+../src/+' EtherTerm.mk"
    sh 'make clean'
    sh 'make -j3'
}
