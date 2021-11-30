import boto3
import json

print('Loading function')


def handler(event, context):
	client = boto3.client('iot-data')
	dynamodb = boto3.client('dynamodb')
	teste = dynamodb.scan(TableName='glucose')
	data = dynamodb.query(
	    TableName='glucose',
	    KeyConditionExpression='#name = :value',
	    ExpressionAttributeValues={
	      ':value': {
	        'S': 'Vinicius Senger'
	      }
	    },
	    ExpressionAttributeNames={
	      '#name': 'name'
	    },
        Limit=1,
        ScanIndexForward=True
	)	
	topicValue = event['queryStringParameters']['topic']
	messageValue = event['queryStringParameters']['message']
	#print(event)
	client.publish(topic=topicValue, payload=messageValue)
	content = {
        "temperature": "20"
    }
	return {
       'statusCode': 200,
       'body': json.dumps(data)
   }
