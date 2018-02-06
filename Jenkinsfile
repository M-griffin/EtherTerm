#!/usr/bin/env groovy
 
import hudson.model.*
import hudson.EnvVars
import groovy.json.JsonSlurperClassic
import groovy.json.JsonBuilder
import groovy.json.JsonOutput
import java.net.URL

pipeline {
	agent any
	stages {
		stage ('git'){
            		steps {
				checkout([
				    $class: 'GitSCM',
				    branches: scm.branches,
				    doGenerateSubmoduleConfigurations: false,
				    extensions: scm.extensions + [[$class: 'SubmoduleOption', disableSubmodules: false, recursiveSubmodules: true, reference: '', trackingSubmodules: false]],
				    submoduleCfg: [],
				    userRemoteConfigs: scm.userRemoteConfigs])
			}
	        }
		
		stage('\u2776 Build') {
			 steps {
				//checkout scm				
				sh '''sed -i 's+/home/blue/code/EtherTerm/src/+../src/+' linux/EtherTerm.mk'''
				sh '''sed -i 's+/home/merc/code/EtherTerm/src/+../src/+' linux/EtherTerm.mk'''
				dir 'linux'
				pwd
				sh 'make -f linux/Makefile clean'
				sh 'make -f linux/Makefile -j3'
			 }
		}
	}
}
