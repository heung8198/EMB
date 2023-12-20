import argparse
import cv2
from ml import Movenet
import utils
def run(estimation_model, camera_id, width, height):
    # Initialize the pose estimator
    pose_detector = Movenet(estimation_model)

    cap = cv2.VideoCapture(camera_id)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)

    while cap.isOpened():
        success, image = cap.read()
        if not success:
            break

        image = cv2.flip(image, 1)
        person = pose_detector.detect(image)
        image = utils.visualize(image, [person])
        
        # 양손이 어깨보다 높은지 확인
        left_shoulder = person.keypoints[5].coordinate # 왼쪽 어깨 좌표
        right_shoulder = person.keypoints[6].coordinate # 오른쪽 어깨 좌표
        left_wrist = person.keypoints[9].coordinate # 왼쪽 손목 좌표
        right_wrist = person.keypoints[10].coordinate # 오른쪽 손목 좌표
        # Delay 추가, 3초
            
        class_name = "non_handup"
        cnt_handup = 0
        cnt_right_hand_up = 0
        cnt_left_hand_up = 0
        

        if left_wrist.y < left_shoulder.y and right_wrist.y < right_shoulder.y:
            cnt_handup += 1
            cnt_right_hand_up = 0
            cnt_left_hand_up = 0
            current_class_name = "hand up"
            if cnt_handup >= 10:
                cap.release()
                cv2.destroyAllWindows()
                return current_class_name
        elif left_wrist.y < left_shoulder.y:
            cnt_left_hand_up += 1
            cnt_handup = 0
            cnt_right_hand_up = 0
            current_class_name = "left_hand_up"
            if cnt_left_hand_up >= 10:
                cap.release()
                cv2.destroyAllWindows()
                return current_class_name
        elif right_wrist.y < right_shoulder.y:
            cnt_right_hand_up += 1
            cnt_handup = 0
            cnt_left_hand_up = 0
            current_class_name = "right_hand_up"
            if cnt_right_hand_up >= 10:
                cap.release()
                cv2.destroyAllWindows()
                return current_class_name
        else:
            cnt_handup = 0
            cnt_right_hand_up = 0
            cnt_left_hand_up = 0
            current_class_name = "non_handup"
        cv2.putText(image, current_class_name, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
        cv2.imshow(estimation_model, image)
        
        # 'q' 키를 누르면 종료
        if cv2.waitKey(1) & 0xFF == ord('q') or cv2.waitKey(1) & 0xFF == ord('Q'):
            break

    cap.release()
    cv2.destroyAllWindows()

# main() 함수와 나머지 코드는 그대로 유지됩니다.

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--model', default='movenet_thunder')
    parser.add_argument('--cameraId', default=0)
    parser.add_argument('--frameWidth', default=640)
    parser.add_argument('--frameHeight', default=480)
    args = parser.parse_args()

    class_detect = run(args.model, int(args.cameraId), int(args.frameWidth), int(args.frameHeight))
    return class_detect
if __name__ == '__main__':
    result = main()
    print(result)
