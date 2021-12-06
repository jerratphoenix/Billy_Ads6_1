// parameters: static_analysis, target

pipeline {
    agent any
    stages{
        stage('Initialize') {
            steps {
	    	  echo "Initialize"

		  sh "cd ${WORKSPACE}; \
                      mkdir -p workspace/robotframework; \
                     "
                  checkout([
                          $class: 'GitSCM',
                          branches: [[name: '*/master']],
                          extensions: [[$class: 'RelativeTargetDirectory', relativeTargetDir: 'workspace/robotframework'],
                                       [$class: 'LocalBranch', localBranch: '**']],
                          userRemoteConfigs: [[credentialsId: 'jenkins-slave-1', url: 'git@github.com:pteceng/RobotFrameworkTest-BMC.git']]
                          ]);

            }
        }
        stage('Static Code Analysis') {
            when {
                expression { return params.static_analysis }
            }
            steps {
                echo "Step STATIC CODE ANALYSIS"

		echo "Initialize and Run Pylint"
                      sh "cd ${WORKSPACE}; \
                      cp ${WORKSPACE}/../pylintrc .; \
                      cp ${WORKSPACE}/../tasks.py .; \
                      invoke pylint > ${WORKSPACE}/pylint1.log; \
                     "

                echo "Pull all modules, Run CPPcheck"
                sh "cd ${WORKSPACE}/; \
                    source setup ${params.target}; \
                    devtool modify at-scale-debug; \
                    devtool modify bmcweb; \
                    devtool modify entity-manager; \
                    devtool modify ffdc; \
                    devtool modify gpioplus; \
                    devtool modify intel-dbus-interfaces; \
                    devtool modify intel-ipmi-oem; \
                    devtool modify ipmi-blob-tool; \
                    devtool modify jsnbd; \
                    devtool modify libpeci; \
                    devtool modify mrw-patch-native; \
                    devtool modify mrw-perl-tools-native; \
                    devtool modify nativesdk-python3-sdbus++; \
                    devtool modify phosphor-ipmi-fru-hostfw-config-example-native; \
                    devtool modify phosphor-ipmi-fru-inventory-example-native; \
                    devtool modify phosphor-ipmi-fru-read-inventory-example-native; \
                    devtool modify phosphor-led-manager-config-example-native; \
                    devtool modify phosphor-led-manager-error-native; \
                    devtool modify phosphor-software-manager-error-native; \
                    devtool modify webui-vue; \
		   "
		script {
		    if (${params.target} == 'archercity') {
		      sh "cd ${WORKSPACE}; \
		          source setup ${params.target}; \
			  devtool modify archercity-led-manager-config-native; \
			 "
		    }
		    else {
		      sh "cd ${WORKSPACE}; \
		          source setup ${params.target}; \
			  devtool modify nativesdk-phosphor-dbus-interfaces; \
			  devtool modify nativesdk-phosphor-logging; \
			 "
		    }
		}
	        script {
                    if (params.linux_kernel == true) {
                       sh "cd ${WORKSPACE}; \
                           source setup ${params.target}; \
                           devtool modify linux-aspeed;"
                    }
                }
                sh "cd ${WORKSPACE}; \
                    cppcheck -j 10 --library=googletest --xml --xml-version=2 \
                             -i ${WORKSPACE}/meta-arm \
                             -i ${WORKSPACE}/meta-aspeed \
                             -i ${WORKSPACE}/meta-bytedance \
                             -i ${WORKSPACE}/meta-evb \
                             -i ${WORKSPACE}/meta-facebook \
                             -i ${WORKSPACE}/meta-google \
                             -i ${WORKSPACE}/meta-hxt \
                             -i ${WORKSPACE}/meta-ibm \
                             -i ${WORKSPACE}/meta-ingrasys \
                             -i ${WORKSPACE}/meta-inspur \
                             -i ${WORKSPACE}/meta-intel \
                             -i ${WORKSPACE}/meta-inventec \
                             -i ${WORKSPACE}/meta-lenovo \
                             -i ${WORKSPACE}/meta-mellanox \
                             -i ${WORKSPACE}/meta-microsoft \
                             -i ${WORKSPACE}/meta-nuvoton \
                             -i ${WORKSPACE}/meta-openembedded \
                             -i ${WORKSPACE}/meta-openpower \
                             -i ${WORKSPACE}/meta-portwell \
                             -i ${WORKSPACE}/meta-qualcomm \
                             -i ${WORKSPACE}/meta-quanta \
                             -i ${WORKSPACE}/meta-raspberrypi \
                             -i ${WORKSPACE}/meta-security \
                             -i ${WORKSPACE}/meta-x86 \
                             -i ${WORKSPACE}/meta-xilinx \
                             -i ${WORKSPACE}/meta-yadro \
                             -i ${WORKSPACE}/build/${params.target}/tmp \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/**/test \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/**/tests \
			     -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/alpha \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/arc \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/c6x \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/csky \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/h8300 \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/hexagon \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/ia64 \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/m68k \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/microblaze \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/mips \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/nds32 \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/nios2 \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/openrisc \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/parisc \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/powerpc \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/riscv \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/s390 \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/sh \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/sparc \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/um \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/unicore32 \
                             -i ${WORKSPACE}/build/${params.target}/workspace/sources/linux-aspeed/arch/xtensa \
                             ${WORKSPACE} 2>${WORKSPACE}/cppcheck.xml; \
			     xmlstarlet ed -O -L -d '//error[@id=\"syntaxError\"][contains(@file0, \"unpack_properties.cpp\")][child::location[@line=\"48\"]]' ${WORKSPACE}/cppcheck.xml; \
                             xmlstarlet ed -O -L -d '//error[@id=\"syntaxError\"][contains(@file0, \"event.cpp\")][child::location[@line=\"141\"]]' ${WORKSPACE}/cppcheck.xml; \
                   "
		echo "Initialize and run ESLint"
		sh "cd ${WORKSPACE}/build/${params.target}/workspace/sources/webui-vue; \
                    sudo npm ci; \
                    eslint --no-error-on-unmatched-pattern --ext .js,.vue -f checkstyle ${WORKSPACE}/build/${params.target}/workspace/sources/webui-vue > ${WORKSPACE}/eslint.xml || true; \
                   "

                echo "Initialize and Run RFLint"
                sh "cd ${WORKSPACE}; \
                    rflint -r ${WORKSPACE}/workspace/robotframework > ${WORKSPACE}/rflint.txt || true; \
                   "

                echo "Initialize and Run Pylint (part 2)"
                sh "cd ${WORKSPACE}/build/${params.target}/workspace/sources; \
                    cp ${WORKSPACE}/../pylintrc .; \
                    cp ${WORKSPACE}/../tasks.py .; \
                    invoke pylint > ${WORKSPACE}/pylint2.log; \
                    cd ${WORKSPACE}; \
                    touch pylint.log; \
                    cat pylint1.log >> pylint.log; \
                    cat pylint2.log >> pylint.log; \
                   "



                // This is using the Warnings Next Generation Plugin.
                script {
                    echo "CPP Scan:"
                    def cppScan = scanForIssues tool: cppCheck(pattern: 'cppcheck.xml')
                    publishIssues issues: [cppScan]
			
		    echo "JS Scan:"
                    def jsScan = scanForIssues tool: esLint(pattern: 'eslint.xml')
                    publishIssues issues: [jsScan]

		    echo "RF Scan:"
                    def rfScan = scanForIssues tool: rfLint(pattern: 'rflint.txt')
                    publishIssues issues: [rfScan]

		    echo "PY Scan:"
                    def pyScan = scanForIssues tool: pyLint(pattern: 'pylint.log')
                    publishIssues issues: [pyScan]
                }
            }
        }

        stage('Build') {
            steps {
                echo "Step Build"
                sh "cd ${WORKSPACE}/; \
                    source setup ${params.target}; \
                    echo 'EXTRA_IMAGE_FEATURES += \"debug-tweaks\"' >> conf/local.conf; \
                    bitbake obmc-phosphor-image \
                   "
            }
        }
    }
    post {
        always {
            echo "POST"

            echo "Archive Artifacts"
	    archiveArtifacts artifacts: 'build/*/tmp/deploy/images/*/*.mtd*', onlyIfSuccessful: true
            script {
            	   if (params.static_analysis == true) {
            	       archiveArtifacts artifacts: 'cppcheck.xml'
		       archiveArtifacts artifacts: 'eslint.xml'
		       archiveArtifacts artifacts: 'rflint.txt'
		       archiveArtifacts artifacts: 'pylint.log'
		   }
            }
	    
            echo "Clean Workspace"
            cleanWs()
        }
    }
}
