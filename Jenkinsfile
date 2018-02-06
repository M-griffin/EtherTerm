#!/usr/bin/env groovy
 
import hudson.model.*
import hudson.EnvVars
import groovy.json.JsonSlurperClassic
import groovy.json.JsonBuilder
import groovy.json.JsonOutput
import java.net.URL
 
try {
	node {
		
			stage('\u2776 Stage 1') {
		
					echo "\u2600 BUILD_URL=${env.BUILD_URL}"

					def workspace = pwd()
					echo "\u2600 workspace=${workspace}"
					checkout scm
		
			}

			stage('\u2777 Stage 2') {
		
					echo 'Update Makefile..'
					dir "linux"
					sh "ls -alh"
					sh "pwd"
					sh "sed -i 's+/home/blue/code/EtherTerm/src/+../src/+' EtherTerm.mk"
					sh "sed -i 's+/home/merc/code/EtherTerm/src/+../src/+' EtherTerm.mk"
					sh "make clean"

					echo 'Building..'
					sh "make -j3"
		
			}
		
			echo "done!"
		}
		
		
		
	} // node
} // try end

catch (exc) {
	/*
	 err = caughtError
	 currentBuild.result = "FAILURE"
	 String recipient = 'infra@lists.jenkins-ci.org'
	 mail subject: "${env.JOB_NAME} (${env.BUILD_NUMBER}) failed",
			 body: "It appears that ${env.BUILD_URL} is failing, somebody should do something about that",
			   to: recipient,
		  replyTo: recipient,
	 from: 'noreply@ci.jenkins.io'
	*/
} finally {
 
	
 (currentBuild.result != "ABORTED") && node("master") {
     // Send e-mail notifications for failed or unstable builds.
     // currentBuild.result must be non-null for this step to work.
	 /*
     step([$class: 'Mailer',
        notifyEveryUnstableBuild: true,
        recipients: "${email_to}",
        sendToIndividuals: true])*/
 }
 
	// Must re-throw exception to propagate error:
	/*
 if (err) {
     throw err
 }*/
}


