import argparse
import cv2
from ml import Movenet
from ml import Classifier
import utils

def run(estimation_model, classification_model, label_file, camera_id, width, height):
    # Initialize the pose estimator and classifier
    pose_detector = Movenet(estimation_model)
    classifier = Classifier(classification_model, label_file)

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
        
        # Run pose classification
        prob_list = classifier.classify_pose(person)
        class_name = prob_list[0].label
        cv2.putText(image, class_name, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
        
        # Calculation pose classification
        # 양손이 어깨보다 높은지 확인
        
        left_shoulder = person.keypoints[5][:2] # 왼쪽 어깨 좌표
        right_shoulder = person.keypoints[6][:2] # 오른쪽 어깨 좌표
        left_wrist = person.keypoints[9][:2] # 왼쪽 손목 좌표
        right_wrist = person.keypoints[10][:2] # 오른쪽 손목 좌표

        if left_wrist < left_shoulder and right_wrist < right_shoulder:
            class_name = "Hands Up"
        else:
            prob_list = classifier.classify_pose(person)
            class_name = prob_list[0].label
            cv2.putText(image, class_name, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
        
        cv2.imshow(estimation_model, image)
        if cv2.waitKey(1) == 27:
            break

    cap.release()
    cv2.destroyAllWindows()

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--model', default='movenet_thunder')
    parser.add_argument('--classifier', default='pose_classifier.tflite')
    parser.add_argument('--label_file', default='labels.txt')
    parser.add_argument('--cameraId', default=0)
    parser.add_argument('--frameWidth', default=640)
    parser.add_argument('--frameHeight', default=480)
    args = parser.parse_args()

    run(args.model, args.classifier, args.label_file, int(args.cameraId), int(args.frameWidth), int(args.frameHeight))

if __name__ == '__main__':
    main()
