import cv2
import numpy as np
from tflite_runtime.interpreter import Interpreter
from threading import Thread

# MoveNet 모델 경로
MOVENET_MODEL_PATH = 'movenet_thunder.tflite'

# 포즈 분류 모델 경로
POSE_CLASSIFIER_MODEL_PATH = 'pose_classifier.tflite'

# 클래스 라벨
LABELS = ['handup', 'non_handup']

# TensorFlow Lite 인터프리터 초기화
movenet_interpreter = Interpreter(model_path=MOVENET_MODEL_PATH)
movenet_interpreter.allocate_tensors()
pose_classifier_interpreter = Interpreter(model_path=POSE_CLASSIFIER_MODEL_PATH)
pose_classifier_interpreter.allocate_tensors()

# 비디오 캡처 시작
cap = cv2.VideoCapture(0)
def preprocess_input(frame, input_size):
    # 프레임을 모델 입력 크기로 조정하고 정규화
    frame_resized = cv2.resize(frame, input_size)
    frame_normalized = frame_resized / 255.0
    return np.expand_dims(frame_normalized, axis=0)

def postprocess_output(landmarks, output_stride):
    # MoveNet 출력을 사후 처리하여 실제 이미지 좌표로 변환
    return landmarks * output_stride

def draw_skeleton(frame, landmarks):
    # landmarks에 따라 스켈레톤 그리기
    # 예: cv2.line(frame, start_point, end_point, color, thickness)
    pass

# MoveNet 모델 입력 크기 및 출력 스트라이드
input_size = (192, 192)
output_stride = 32

try:
    while True:
        ret, frame = cap.read()
        if not ret:
            break

        # MoveNet 추론 실행
        input_data = preprocess_input(frame, input_size)
        movenet_interpreter.set_tensor(movenet_interpreter.get_input_details()[0]['index'], input_data)
        movenet_interpreter.invoke()
        landmarks = postprocess_output(movenet_interpreter.get_tensor(movenet_interpreter.get_output_details()[0]['index'])[0], output_stride)

        # 포즈 분류 모델 추론 실행
        pose_classifier_interpreter.set_tensor(pose_classifier_interpreter.get_input_details()[0]['index'], landmarks)
        pose_classifier_interpreter.invoke()
        predicted_label = LABELS[np.argmax(pose_classifier_interpreter.get_tensor(pose_classifier_interpreter.get_output_details()[0]['index'])[0])]

        # 스켈레톤 및 분류 결과 그리기
        draw_skeleton(frame, landmarks)
        cv2.putText(frame, predicted_label, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
        cv2.imshow('Pose Estimation', frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
except KeyboardInterrupt:
    pass
finally:
    cap.release()
    cv2.destroyAllWindows()
