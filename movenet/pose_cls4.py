import argparse
import cv2
from ml import Movenet
import utils

def run(estimation_model, camera_id, width, height):
    pose_detector = Movenet(estimation_model)
    cap = cv2.VideoCapture(camera_id)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)

    cnt_handup = 0
    cnt_right_hand_up = 0
    cnt_left_hand_up = 0

    while cap.isOpened():
        success, image = cap.read()
        if not success:
            break

        image = cv2.flip(image, 1)
        person = pose_detector.detect(image)
        image = utils.visualize(image, [person])

        left_shoulder = person.keypoints[5].coordinate
        right_shoulder = person.keypoints[6].coordinate
        left_wrist = person.keypoints[9].coordinate
        right_wrist = person.keypoints[10].coordinate
        current_class = "non"
        
        if left_wrist.y < left_shoulder.y and right_wrist.y < right_shoulder.y:
            cnt_handup += 1
            current_class = "hand up"
            if cnt_handup >= 10:
                cap.release()
                cv2.destroyAllWindows()
                return "hand up"
        elif left_wrist.y < left_shoulder.y:
            cnt_left_hand_up += 1
            current_class = "left_hand_up"
            if cnt_left_hand_up >= 10:
                cap.release()
                cv2.destroyAllWindows()
                return "left_hand_up"
        elif right_wrist.y < right_shoulder.y:
            cnt_right_hand_up += 1
            current_class = "right_hand_up"
            if cnt_right_hand_up >= 10:
                cap.release()
                cv2.destroyAllWindows()
                return "right_hand_up"
        else:
            cnt_handup = 0
            cnt_right_hand_up = 0
            cnt_left_hand_up = 0

        cv2.putText(image, current_class, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
        cv2.imshow(estimation_model, image)

        if cv2.waitKey(1) & 0xFF in [ord('q'), ord('Q')]:
            break

    cap.release()
    cv2.destroyAllWindows()
    return None

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--model', default='4.tflite')
    parser.add_argument('--cameraId', default=0)
    parser.add_argument('--frameWidth', default=640)
    parser.add_argument('--frameHeight', default=480)
    args = parser.parse_args()

    detected_class = run(args.model, int(args.cameraId), int(args.frameWidth), int(args.frameHeight))
    if detected_class:
        print(detected_class)

if __name__ == '__main__':
    main()
