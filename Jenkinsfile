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
		stage('\u2776 Checkout') {
			 steps {
				echo "\u2600 BUILD_URL=${env.BUILD_URL}"

				//def workspace = pwd()
				//echo "\u2600 workspace=${workspace}"
				checkout scm
			 }
		}

		stage('\u2777 MakeFile') {
			 steps {
				echo 'Update Makefile..'
				dir 'linux'
				sh 'ls -alh'
				sh 'pwd'
				sh '''sed -i 's+/home/blue/code/EtherTerm/src/+../src/+' EtherTerm.mk'''
				sh '''sed -i 's+/home/merc/code/EtherTerm/src/+../src/+' EtherTerm.mk'''
			 }
		}

		stage('\u2777 Build') {
			 steps {
				echo 'Building..'
				sh 'make clean'
				sh 'make -j3'
			 }
		}

	}
}
