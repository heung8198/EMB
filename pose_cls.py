import cv2
import numpy as np
from tflite_runtime.interpreter import Interpreter
from threading import Thread

# MoveNet 모델 경로
MOVENET_MODEL_PATH = 'path/to/movenet_model.tflite'

# 포즈 분류 모델 경로
POSE_CLASSIFIER_MODEL_PATH = 'path/to/pose_classifier_model.tflite'

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
    # 프레임을 모델 입력 크기로 조정
    frame_resized = cv2.resize(frame, input_size)
    
    # 이미지 데이터를 UINT8로 변환
    input_data = np.uint8(frame_resized)
    
    return np.expand_dims(input_data, axis=0)


def preprocess_input(frame, input_size):
    # 프레임을 모델 입력 크기로 조정하고 정규화
    frame_resized = cv2.resize(frame, input_size)
    frame_normalized = frame_resized / 255.0
    return np.expand_dims(frame_normalized, axis=0)

def extract_movenet_landmarks(interpreter, output_stride):
    """MoveNet 출력에서 랜드마크 추출"""
    landmarks = interpreter.get_tensor(interpreter.get_output_details()[0]['index'])[0]
    return landmarks * output_stride

def draw_skeleton(frame, landmarks):
    """랜드마크를 사용하여 스켈레톤 그리기"""
    joints = landmarks.reshape(-1, 3)  # 각 관절의 (y, x, confidence) 좌표

    # 관절 연결을 위한 인덱스 쌍 (MoveNet에 따라 조정)
    limb_indices = [(0, 1), (1, 2), (2, 3), (3, 4), 
                    (0, 5), (5, 6), (6, 7), (7, 8), 
                    (0, 9), (9, 10), (10, 11), (11, 12), 
                    (0, 13), (13, 14), (14, 15), (15, 16)]

    for limb in limb_indices:
        start_idx, end_idx = limb
        if joints[start_idx, 2] > 0.5 and joints[end_idx, 2] > 0.5:  # 확신도 검사
            start_point = (int(joints[start_idx, 1]), int(joints[start_idx, 0]))
            end_point = (int(joints[end_idx, 1]), int(joints[end_idx, 0]))
            cv2.line(frame, start_point, end_point, (0, 255, 0), 2)

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

        # 랜드마크 추출 및 스켈레톤 그리기
        landmarks = extract_movenet_landmarks(movenet_interpreter, output_stride)
        draw_skeleton(frame, landmarks)
        
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
