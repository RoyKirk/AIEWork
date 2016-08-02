using UnityEngine;
using System.Collections;

public class WhirlpoolOffest : MonoBehaviour
{
	void Update ()
    {
        transform.RotateAround(Vector3.zero, Vector3.up, 20 * Time.deltaTime);
    }
}
