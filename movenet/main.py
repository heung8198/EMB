import subprocess

# 실행 파일을 호출하는 함수를 정의
import subprocess

def run_c_program(program_name, args):
    # 여기에 필요한 커널 모듈을 적재하는 코드를 추가하세요.
    kernel_module = program_name + "_driver.ko"
    subprocess.run(["sudo", "insmod", kernel_module])

    try:
        # 서브프로세스를 실행하고 결과를 얻음
        result = subprocess.run(["sudo", program_name] + args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, check=True)
        return result.stdout, result.stderr
    except subprocess.CalledProcessError as e:
        print(f"Error Occurred: {e}")
        return None, str(e)
    except Exception as e:
        print(f"Exception Occurred: {e}")
        return None, str(e)
    finally:
        # 프로그램 실행이 끝나면 커널 모듈을 제거합니다.
        subprocess.run(["sudo", "rmmod", kernel_module])

# 여기에 C 프로그램 실행 로직을 추가하세요.
# 예: run_c_program("./driver/button", [])

# Python 스크립트를 호출하는 함수
def run_python_script(script_path):
    try:
        # Python 스크립트를 실행하고 결과를 얻음
        result = subprocess.run(["python3", script_path], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, check=True)
        return result.stdout, result.stderr

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
button_stdout, button_stderr = run_c_program("../driver/button", [])

# 디버깅: button 프로그램의 표준 출력과 에러 메시지 확인
print("Debug - Standard Output:", button_stdout)
print("Debug - Error Message:", button_stderr)

# 버튼 상태가 '1'일 때 pose_cls4.py 실행
if button_stdout and "Button 1 (GPIO 16) state: 1" in button_stdout:
    run_python_script("pose_cls4.py")
else:
    print("Button 1 was not pressed or no output was received.")

# 나머지 프로그램을 순서대로 실행
run_c_program("../driver/step_motor", [])  # step_motor 실행
run_c_program("../driver/ultrasonic", [])  # ultrasonic 실행
run_c_program("../driver/led", ["1"])                 # led 실행
