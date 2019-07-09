#!/usr/bin/env groovy

def ircNotification(result) {
  def nick = "XRM-PIPELINE${BUILD_NUMBER}"
  def channel = '#oblivion2'
  result = result.toUpperCase()
  def message = "Project ${JOB_NAME} build #${BUILD_NUMBER}: ${result}: ${BUILD_URL}"
  node {
    sh """
        (
        echo NICK ${nick}
        echo USER ${nick} 8 * : ${nick}
        sleep 5
        echo "JOIN ${channel}"
        echo "NOTICE ${channel} :${message}"
        echo QUIT
        ) | openssl s_client -connect irc.freenode.net:6697
    """
  }
}

pipeline {
  agent any
  stages {
    stage('\u2776 Build') {
      steps {								
	sh '''sed -i 's+/home/blue/code/EtherTerm/src/+../src/+' linux/EtherTerm.mk'''
	sh '''sed -i 's+/home/merc/code/EtherTerm/src/+../src/+' linux/EtherTerm.mk'''
	dir ('linux') {
	  sh 'make -f Makefile clean'
	  sh 'make -f Makefile -j3'
        }
      }
    }
  }
  post {
    change {
      ircNotification(currentBuild.result)
    }
  }
}
