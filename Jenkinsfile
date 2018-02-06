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
		stage('\u2776 Build') {
			 steps {
				checkout scm
				dir 'linux'
				sh '''sed -i 's+/home/blue/code/EtherTerm/src/+../src/+' EtherTerm.mk'''
				sh '''sed -i 's+/home/merc/code/EtherTerm/src/+../src/+' EtherTerm.mk'''
				sh 'make clean'
				sh 'make -j3'
			 }
		}
	}
}
