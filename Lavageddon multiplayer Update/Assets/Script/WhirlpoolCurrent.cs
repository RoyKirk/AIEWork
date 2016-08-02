using UnityEngine;
using System.Collections;

public class WhirlpoolCurrent : MonoBehaviour
{

    GameObject cube;
    Transform center;
    Vector3 axis = Vector3.up;
    Vector3 desiredPosition;

    float radius = 2.0f;
    public float radiusSpeed = 1f;
    public float rotationSpeed = 5.0f;
    
    void Start()
    {
        cube = GameObject.FindWithTag("Whirlpool");
        center = cube.transform;
    }

    void Update()
    {
        transform.RotateAround(center.position, axis, rotationSpeed * Time.deltaTime);
        desiredPosition = (transform.position - center.position).normalized * radius + center.position;
        transform.position = Vector3.MoveTowards(transform.position, new Vector3(desiredPosition.x, this.transform.position.y,desiredPosition.z), Time.deltaTime * radiusSpeed);
    }
}