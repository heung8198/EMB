import subprocess

# 실행 파일을 호출하는 함수를 정의
def run_c_program(program_name, args):
    try:
        # 서브프로세스를 실행하고 결과를 얻음
        result = subprocess.run([program_name] + args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, check=True)

        # 표준 출력과 에러 메시지 반환
        return result.stdout, result.stderr
    except subprocess.CalledProcessError as e:
        print(f"Error Occurred: {e}")
        return None, str(e)
    except Exception as e:
        print(f"Exception Occurred: {e}")
        return None, str(e)

# Python 스크립트를 호출하는 함수
def run_python_script(script_path):
    try:
        # Python 스크립트를 실행하고 결과를 얻음
        result = subprocess.run(["python", script_path], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, check=True)

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

# button 프로그램을 실행하고, 그 출력을 분석
button_stdout, button_stderr = run_c_program("./drive/button", ["arg1"])

# 버튼 상태가 '1'일 때 pose_cls4.py 실행
if "Button 1 (GPIO 16) state: 1" in button_stdout:
    run_python_script("./movenet/pose_cls4.py")

# 여기에 나머지 프로그램 실행 로직을 추가하세요.

run_c_program("./step_motor", ["arg1", "arg2"])  # step_motor 실행
run_c_program("./ultrasonic", ["arg1", "arg2"])  # ultrasonic 실행
run_c_program("./led", ["arg1"])                 # led 실행
