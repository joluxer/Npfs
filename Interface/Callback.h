/*
 * Callback.h
 *
 */

#ifndef CALLBACK_H_
#define CALLBACK_H_

template<typename ReturnType, typename T1 = void, typename T2 = void, typename T3 = void, typename T4 = void>
class Callback
{
public:
  //  Callback()
  //  {}

  virtual ~Callback()
  {
  }

  virtual ReturnType send(T1 p1, T2 p2, T3 p3, T4 p4) const =0;

  ReturnType execute(T1 parameter1, T2 parameter2, T3 parameter3, T4 parameter4) const
  {
    return send(parameter1, parameter2, parameter3, parameter4);
  }

  ReturnType notify(T1 parameter1, T2 parameter2, T3 parameter3, T4 parameter4) const
  {
    return send(parameter1, parameter2, parameter3, parameter4);
  }
};

template<typename ReturnType, typename T1, typename T2, typename T3>
class Callback<ReturnType, T1, T2, T3>
{
public:
  //  Callback()
  //  {}

  virtual ~Callback()
  {
  }

  virtual ReturnType send(T1 p1, T2 p2, T3 p3) const =0;

  ReturnType execute(T1 parameter1, T2 parameter2, T3 parameter3) const
  {
    return send(parameter1, parameter2, parameter3);
  }

  ReturnType notify(T1 parameter1, T2 parameter2, T3 parameter3) const
  {
    return send(parameter1, parameter2, parameter3);
  }
};

template<typename ReturnType, typename T1, typename T2>
class Callback<ReturnType, T1, T2, void>
{
public:
  //  Callback()
  //  {}

  virtual ~Callback()
  {
  }

  virtual ReturnType send(T1 p1, T2 p2) const =0;

  ReturnType execute(T1 parameter1, T2 parameter2) const
  {
    return send(parameter1, parameter2);
  }

  ReturnType notify(T1 parameter1, T2 parameter2) const
  {
    return send(parameter1, parameter2);
  }
};

template<typename ReturnType, typename T1>
class Callback<ReturnType, T1, void>
{
public:
  //  Callback()
  //  {}

  virtual ~Callback()
  {
  }

  virtual ReturnType send(T1 p1) const =0;

  ReturnType execute(T1 parameter1) const
  {
    return send(parameter1);
  }

  ReturnType notify(T1 parameter1) const
  {
    return send(parameter1);
  }
};

template<typename ReturnType>
class Callback<ReturnType, void>
{
public:
  //  Callback()
  //  {}

  virtual ~Callback()
  {
  }

  virtual ReturnType send() const =0;

  ReturnType execute() const
  {
    return send();
  }

  ReturnType notify() const
  {
    return send();
  }
};

template<class Class, typename ReturnType, typename Parameter1 = void, typename Parameter2 = void, typename Parameter3 = void,
    typename Parameter4 = void>
class CalleeMember: public Callback<ReturnType, Parameter1, Parameter2, Parameter3, Parameter4>
{

public:
  typedef ReturnType
  (Class::*MemberFunctionPtr)(Parameter1, Parameter2, Parameter3, Parameter4);

  CalleeMember(Class* _class_instance, MemberFunctionPtr _method) :
    callee(_class_instance), memberFunction(_method)
  {
  }

  ReturnType send(Parameter1 parameter1, Parameter2 parameter2, Parameter3 parameter3, Parameter4 parameter4) const
  {
    return (callee->*memberFunction)(parameter1, parameter2, parameter3, parameter4);
  }

  ReturnType execute(Parameter1 parameter1, Parameter2 parameter2, Parameter3 parameter3, Parameter4 parameter4) const
  {
    return send(parameter1, parameter2, parameter3, parameter4);
  }

  ReturnType notify(Parameter1 parameter1, Parameter2 parameter2, Parameter3 parameter3, Parameter4 parameter4) const
  {
    return send(parameter1, parameter2, parameter3, parameter4);
  }

private:
  Class * const callee;
  const MemberFunctionPtr memberFunction;

};

template<class Class, typename ReturnType, typename Parameter1, typename Parameter2, typename Parameter3>
class CalleeMember<Class, ReturnType, Parameter1, Parameter2, Parameter3> : public Callback<ReturnType, Parameter1, Parameter2,
    Parameter3>
{

public:
  typedef ReturnType
  (Class::*MemberFunctionPtr)(Parameter1, Parameter2, Parameter3);

  CalleeMember(Class* _class_instance, MemberFunctionPtr _method) :
    callee(_class_instance), memberFunction(_method)
  {
  }

  ReturnType send(Parameter1 parameter1, Parameter2 parameter2, Parameter3 parameter3) const
  {
    return (callee->*memberFunction)(parameter1, parameter2, parameter3);
  }

  ReturnType execute(Parameter1 parameter1, Parameter2 parameter2, Parameter3 parameter3) const
  {
    return send(parameter1, parameter2, parameter3);
  }

  ReturnType notify(Parameter1 parameter1, Parameter2 parameter2, Parameter3 parameter3) const
  {
    return send(parameter1, parameter2, parameter3);
  }

private:
  Class * const callee;
  const MemberFunctionPtr memberFunction;

};

template<class Class, typename ReturnType, typename Parameter1, typename Parameter2>
class CalleeMember<Class, ReturnType, Parameter1, Parameter2, void> : public Callback<ReturnType, Parameter1, Parameter2>
{

public:
  typedef ReturnType
  (Class::*MemberFunctionPtr)(Parameter1, Parameter2);

  CalleeMember(Class* _class_instance, MemberFunctionPtr _method) :
    callee(_class_instance), memberFunction(_method)
  {
  }

  ReturnType send(Parameter1 parameter1, Parameter2 parameter2) const
  {
    return (callee->*memberFunction)(parameter1, parameter2);
  }

  ReturnType execute(Parameter1 parameter1, Parameter2 parameter2) const
  {
    return send(parameter1, parameter2);
  }

  ReturnType notify(Parameter1 parameter1, Parameter2 parameter2) const
  {
    return send(parameter1, parameter2);
  }

private:
  Class * const callee;
  const MemberFunctionPtr memberFunction;

};

template<class Class, typename ReturnType, typename Parameter>
class CalleeMember<Class, ReturnType, Parameter, void> : public Callback<ReturnType, Parameter>
{

public:
  typedef ReturnType
  (Class::*MemberFunctionPtr)(Parameter);

  CalleeMember(Class* _class_instance, MemberFunctionPtr _method) :
    callee(_class_instance), memberFunction(_method)
  {
  }

  ReturnType send(Parameter parameter) const
  {
    return (callee->*memberFunction)(parameter);
  }

  ReturnType execute(Parameter parameter) const
  {
    return send(parameter);
  }

  ReturnType notify(Parameter parameter) const
  {
    return send(parameter);
  }

private:
  Class * const callee;
  const MemberFunctionPtr memberFunction;

};

template<class Class, typename ReturnType>
class CalleeMember<Class, ReturnType, void> : public Callback<ReturnType>
{

public:
  typedef ReturnType
  (Class::*MemberFunctionPtr)(void);

  CalleeMember(Class* _class_instance, MemberFunctionPtr _method) :
    callee(_class_instance), memberFunction(_method)
  {
  }

  ReturnType send() const
  {
    return (callee->*memberFunction)();
  }

  ReturnType execute() const
  {
    return send();
  }

  ReturnType notify() const
  {
    return send();
  }

private:
  Class * const callee;
  const MemberFunctionPtr memberFunction;

};

#endif /* CALLBACK_H_ */
