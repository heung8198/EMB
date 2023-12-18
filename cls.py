import cv2
import numpy as np
from threading import Thread
from tflite_runtime.interpreter import Interpreter
import time
class VideoStream:
    """카메라 객체가 비디오 스트리밍을 제어합니다."""
    def __init__(self, resolution=(640, 480), framerate=30):
        # 카메라 초기화
        self.stream = cv2.VideoCapture(0)
        ret = self.stream.set(3, resolution[0])
        ret = self.stream.set(4, resolution[1])

        (self.grabbed, self.frame) = self.stream.read()

        # 스트림이 멈췄는지 확인하는 변수
        self.stopped = False

    def start(self):
        # 비디오 스트림을 읽는 스레드 시작
        Thread(target=self.update, args=()).start()
        return self

    def update(self):
        # 스레드가 멈출 때까지 계속 실행
        while True:
            if self.stopped:
                self.stream.release()
                return

            (self.grabbed, self.frame) = self.stream.read()

    def read(self):
        # 가장 최근의 프레임 반환
        return self.frame if self.grabbed else None

    def stop(self):
        # 카메라와 스레드를 멈춤
        self.stopped = True

# 모델 및 라벨 로드
MODEL_PATH = 'kick_up.tflite'
LABELS = ['HUP', 'stand', 'taekwondo']

# TensorFlow Lite 인터프리터 초기화
interpreter = Interpreter(model_path=MODEL_PATH)
interpreter.allocate_tensors()

# 입력 및 출력 세부 정보
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

# 모델 입력 크기
input_shape = input_details[0]['shape']
input_height = input_shape[1]
input_width = input_shape[2]

# 비디오 스트림 초기화 및 시작
videostream = VideoStream(resolution=(640, 480), framerate=30).start()
time.sleep(1)
# ...

# ...
try:
    while True:
        frame = videostream.read()
        if frame is None:
            print("카메라에서 프레임을 읽지 못했습니다.")
            continue  # 다음 프레임을 시도

        # 이미지 전처리: 입력 이미지 크기를 모델의 크기에 맞게 조정
        resized_frame = cv2.resize(frame, (180, 180))  # 180x180 크기로 조정
        input_data = np.expand_dims(resized_frame, axis=0)
        input_data = (np.float32(input_data) - 127.5) / 127.5

        # 모델 추론 실행
        interpreter.set_tensor(input_details[0]['index'], input_data)
        interpreter.invoke()

        # 결과 추출 및 표시
        # ...

# ...

        # 모델 추론
        interpreter.set_tensor(input_details[0]['index'], input_data)
        interpreter.invoke()

        # 결과 추출
        output_data = interpreter.get_tensor(output_details[0]['index'])
        predicted_label = LABELS[np.argmax(output_data)]

        # 결과 표시
        cv2.putText(frame, predicted_label, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
        cv2.imshow('Camera Input', frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
except KeyboardInterrupt:
    pass
finally:
    videostream.stop()
    cv2.destroyAllWindows()

