import subprocess

def run_c_program(program_name, args):
    # 커널 모듈 경로 설정
    kernel_module = program_name + "_driver.ko"
    subprocess.run(["sudo", "insmod", kernel_module])

    try:
        # 서브프로세스 실행
        result = subprocess.run(["sudo", program_name] + args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, check=True)
        return result.stdout, result.stderr
    except subprocess.CalledProcessError as e:
        print(f"Error Occurred: {e}")
        return None, str(e)
    except Exception as e:
        print(f"Exception Occurred: {e}")
        return None, str(e)
    finally:
        # 커널 모듈 제거
        subprocess.run(["sudo", "rmmod", kernel_module])

# 나머지 함수들은 동일하게 유지

# button 프로그램 실행 및 분석
button_stdout, button_stderr = run_c_program("../driver/button", [])

# 버튼 상태 확인 및 pose_cls4.py 실행
if button_stdout and "Button 1 (GPIO 16) state: 1" in button_stdout:
    if check_class_name("pose_cls4.py", "hands_up"):
        run_c_program("../driver/step_motor", ["1"])  # step_motor 실행
    else:
        print("hand up not detected step_motor not run")
else:
    print("Button 1 was not pressed or no output was received.")

# LED 드라이버 모듈 적재
subprocess.run(["sudo", "insmod", "../driver/led_driver.ko"])

# Ultrasonic 프로그램 실행
run_c_program("../driver/ultrasonic", [])

# Button 2 확인
button_stdout, button_stderr = run_c_program("../driver/button", [])
if button_stdout and "Button 2 (GPIO 20) state: 1" in button_stdout:
    subprocess.run(["sudo", "rmmod", "led_driver"])
