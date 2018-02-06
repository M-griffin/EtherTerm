node {
	agent any	
	//echo "Running ${env.BUILD_ID} on ${env.JENKINS_URL}"
	
	stages {	
		stage('Build') {
			steps {
				echo 'Running Checkout..'
				checkout scm
				
				echo 'Update Makefile..'
				sh "cd linux"
				sh "sed -i 's+/home/blue/code/EtherTerm/src/+../src/+ EtherTerm.mk'"
				sh "sed -i 's+/home/merc/code/EtherTerm/src/+../src/+ EtherTerm.mk'"
				sh "make clean"
				
				echo 'Building..'
				sh "make -j3"
			}
		}
	}
}
