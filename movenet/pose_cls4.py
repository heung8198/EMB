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

        if left_wrist.y < left_shoulder.y and right_wrist.y < right_shoulder.y:
            class_name = "hand up"
            # 이하 추가 새로운 코드
            cap.release()
            cv.destroyALLWindows()
            print(class_name)
            return class_name
            # 여기까지
        elif right_wrist.y < right_shoulder.y:
            class_name = "left_hand_up"
            cap.release()
            cv.destroyALLWindows()
            print(class_name)
            return class_name
        elif left_wrist.y < left_shoulder.y:
            class_name = "right_hand_up"
            cap.release()
            cv.destroyALLWindows()
            print(class_name)
            return class_name
        else:
            class_name = "non_handup"

            
        cv2.putText(image, class_name, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
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

    run(args.model, int(args.cameraId), int(args.frameWidth), int(args.frameHeight))

if __name__ == '__main__':
    main()
