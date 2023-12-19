import subprocess

# 실행 파일을 호출하는 함수를 정의
def run_c_program(program_name, args):
    try:
        # 서브프로세스를 실행하고 결과를 얻음
        result = subprocess.run([program_name] + args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, check=True)
        
        # 표준 출력 출력
        print("Standard Output:")  
        print(result.stdout)
        
        # 에러 메시지 출력, 있을 경우
        if result.stderr:
            print("Error Message:")  
            print(result.stderr)
    except subprocess.CalledProcessError as e:
        print(f"Error Occurred: {e}")
    except Exception as e:
        print(f"Exception Occurred: {e}")


# Python 스크립트를 호출하는 함수
def run_python_script(script_name):
    try:
        # Python 스크립트를 실행하고 결과를 얻음
        result = subprocess.run(["python", script_name], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, check=True)

        # Python 스크립트의 표준 출력 출력
        print("Standard Output from Python Script:")
        print(result.stdout)

        # Python 스크립트의 에러 메시지 출력, 있을 경우
        if result.stderr:
            print("Error Message from Python Script:")
            print(result.stderr)
    except subprocess.CalledProcessError as e:
        print(f"Error Occurred in Python Script: {e}")
    except Exception as e:
        print(f"Exception Occurred in Python Script: {e}")
		
# 각 C 프로그램을 호출하는 예제
run_c_program("./button", ["sudo", "arg2"])
run_c_program("./led", ["arg1"])
run_c_program("./ultrasonic", ["arg1", "arg2"])
run_c_program("./step_motor", ["arg1", "arg2"])



# Python 스크립트 호출
run_python_script("pose_cls3.py")

